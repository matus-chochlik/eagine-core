DROP SCHEMA eagilog CASCADE;

CREATE SCHEMA eagilog;

--------------------------------------------------------------------------------
-- statistics
--------------------------------------------------------------------------------
CREATE VIEW eagilog.schema_statistics
AS
SELECT relname
AS
	table_name,
	pg_total_relation_size(c.oid) AS total_bytes,
	pg_size_pretty(pg_total_relation_size(c.oid)) AS pretty_size
FROM pg_class c
LEFT JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE relkind = 'r'
AND nspname = 'eagilog';

CREATE FUNCTION eagilog.schema_data_size_mebi() RETURNS NUMERIC
AS 'SELECT round(sum(total_bytes) / 1048576, 3)::NUMERIC FROM eagilog.schema_statistics;'
LANGUAGE sql IMMUTABLE;
--------------------------------------------------------------------------------
-- severity
--------------------------------------------------------------------------------
CREATE TABLE eagilog.severity (
	severity_id SMALLINT PRIMARY KEY,
	name VARCHAR(9) NOT NULL
);

CREATE UNIQUE INDEX name_index
ON eagilog.severity (name);

CREATE FUNCTION eagilog.get_severity_id(
	eagilog.severity.name%TYPE
) RETURNS eagilog.severity.severity_id%TYPE
AS $$
DECLARE
	result eagilog.severity.severity_id%TYPE;
BEGIN
	SELECT severity_id
	INTO result
	FROM eagilog.severity
	WHERE name = $1;
	return result;
END
$$ LANGUAGE plpgsql;

INSERT INTO eagilog.severity (severity_id, name) VALUES(0, 'backtrace');
INSERT INTO eagilog.severity (severity_id, name) VALUES(1, 'trace');
INSERT INTO eagilog.severity (severity_id, name) VALUES(2, 'debug');
INSERT INTO eagilog.severity (severity_id, name) VALUES(3, 'stat');
INSERT INTO eagilog.severity (severity_id, name) VALUES(4, 'info');
INSERT INTO eagilog.severity (severity_id, name) VALUES(5, 'warning');
INSERT INTO eagilog.severity (severity_id, name) VALUES(6, 'error');
INSERT INTO eagilog.severity (severity_id, name) VALUES(7, 'fatal');
--------------------------------------------------------------------------------
-- message_format
--------------------------------------------------------------------------------
CREATE TABLE eagilog.message_format (
	message_format_id SERIAL PRIMARY KEY,
	hash BYTEA NOT NULL,
	format VARCHAR(160) NOT NULL
);

CREATE UNIQUE INDEX message_format_hash_index
ON eagilog.message_format (hash);

CREATE FUNCTION eagilog.trigger_message_format_hash_default()
RETURNS TRIGGER
AS $$
BEGIN
	IF NEW.hash IS NULL
	THEN NEW.hash = decode(md5(NEW.format), 'hex');
	END IF;
	RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER default_hash_on_insert
BEFORE INSERT OR UPDATE ON eagilog.message_format
FOR EACH ROW EXECUTE PROCEDURE eagilog.trigger_message_format_hash_default();

CREATE FUNCTION eagilog.get_message_format_id(
	eagilog.message_format.hash%TYPE,
	eagilog.message_format.format%TYPE
) RETURNS eagilog.message_format.message_format_id%TYPE
AS $$
DECLARE
	result eagilog.message_format.message_format_id%TYPE;
BEGIN
	LOOP
		BEGIN
			WITH sel AS (
				SELECT lmf.message_format_id
				FROM eagilog.message_format lmf
				WHERE lmf.hash = $1
				FOR SHARE
			)  , ins AS (
				INSERT INTO eagilog.message_format (format)
				SELECT $2
				WHERE NOT EXISTS (SELECT 1 FROM sel)
				RETURNING eagilog.message_format.message_format_id
			)
			SELECT sel.message_format_id FROM sel
			UNION ALL
			SELECT ins.message_format_id FROM ins
			INTO result;
		EXCEPTION WHEN UNIQUE_VIOLATION THEN
			result := NULL;
		END;

		EXIT WHEN result IS NOT NULL;
	END LOOP;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.get_message_format_id(
	eagilog.message_format.format%TYPE
) RETURNS eagilog.message_format.message_format_id%TYPE
AS $$
BEGIN
	RETURN eagilog.get_message_format_id(decode(md5($1), 'hex'), $1);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- stream command
--------------------------------------------------------------------------------
CREATE TABLE eagilog.stream_command (
	stream_command_id SERIAL PRIMARY KEY,
	hash BYTEA NOT NULL,
	command VARCHAR(256) NOT NULL
);

CREATE UNIQUE INDEX stream_command_hash_index
ON eagilog.stream_command (hash);

CREATE FUNCTION eagilog.trigger_stream_command_hash_default()
RETURNS TRIGGER
AS $$
BEGIN
	IF NEW.hash IS NULL
	THEN NEW.hash = decode(md5(NEW.command), 'hex');
	END IF;
	RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER default_hash_on_insert
BEFORE INSERT OR UPDATE ON eagilog.stream_command
FOR EACH ROW EXECUTE PROCEDURE eagilog.trigger_stream_command_hash_default();

CREATE FUNCTION eagilog.get_stream_command_id(
	eagilog.stream_command.hash%TYPE,
	eagilog.stream_command.command%TYPE
) RETURNS eagilog.stream_command.stream_command_id%TYPE
AS $$
DECLARE
	result eagilog.stream_command.stream_command_id%TYPE;
BEGIN
	LOOP
		BEGIN
			WITH sel AS (
				SELECT lmf.stream_command_id
				FROM eagilog.stream_command lmf
				WHERE lmf.hash = $1
				FOR SHARE
			)  , ins AS (
				INSERT INTO eagilog.stream_command (command)
				SELECT $2
				WHERE NOT EXISTS (SELECT 1 FROM sel)
				RETURNING eagilog.stream_command.stream_command_id
			)
			SELECT sel.stream_command_id FROM sel
			UNION ALL
			SELECT ins.stream_command_id FROM ins
			INTO result;
		EXCEPTION WHEN UNIQUE_VIOLATION THEN
			result := NULL;
		END;

		EXIT WHEN result IS NOT NULL;
	END LOOP;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.get_stream_command_id(
	eagilog.stream_command.command%TYPE
) RETURNS eagilog.stream_command.stream_command_id%TYPE
AS $$
BEGIN
	RETURN eagilog.get_stream_command_id(decode(md5($1), 'hex'), $1);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- stream
--------------------------------------------------------------------------------
CREATE TABLE eagilog.stream (
	stream_id SERIAL PRIMARY KEY,
	application_id VARCHAR(10) NULL,
	start_time TIMESTAMP WITH TIME ZONE NOT NULL,
	finish_time TIMESTAMP WITH TIME ZONE NULL,
	stream_command_id INTEGER NULL,
	git_hash VARCHAR(64) NULL,
	git_version VARCHAR(32) NULL,
	os_name VARCHAR(64) NULL,
	hostname VARCHAR(64) NULL,
	architecture VARCHAR(32) NULL,
	compiler VARCHAR(32) NULL,
	low_profile_build BOOL NULL,
	debug_build BOOL NULL
);

CREATE FUNCTION eagilog.start_stream(
) RETURNS eagilog.stream.stream_id%TYPE
AS $$
DECLARE
	result eagilog.stream.stream_id%TYPE;
BEGIN
	WITH ins AS (
		INSERT INTO eagilog.stream (start_time)
		VALUES(now())
		RETURNING eagilog.stream.stream_id
	)
	SELECT stream_id INTO result FROM ins;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.finish_stream(
	eagilog.stream.stream_id%TYPE
) RETURNS eagilog.stream.stream_id%TYPE
AS $$
BEGIN
	UPDATE eagilog.stream
	SET finish_time = now()
	WHERE stream_id = $1;
	RETURN $1;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- stream metadata function
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.set_stream_application_id(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET application_id = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.set_stream_command(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET stream_command_id = eagilog.get_stream_command_id(_value)
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.set_stream_git_hash(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET git_hash = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_git_version(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET git_version = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_os_name(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET os_name = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_hostname(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET hostname = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_architecture(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET architecture = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_compiler(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET compiler = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_low_profile_build(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET low_profile_build = _value::BOOL
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_debug_build(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET debug_build  = _value::BOOL
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- stream views
--------------------------------------------------------------------------------
CREATE VIEW eagilog.stream_command_ref_count
AS
SELECT
    stream_command_id, hash, command,
    count(stream_id) AS ref_count
FROM eagilog.stream_command
JOIN eagilog.stream
USING (stream_command_id)
GROUP BY (stream_command_id, hash, command);
--------------------------------------------------------------------------------
CREATE VIEW eagilog.finished_stream
AS
SELECT
	stream_id, application_id,
	start_time, finish_time,
	finish_time - start_time AS duration,
	command,
	git_hash,
	git_version,
	os_name,
	hostname,
	architecture,
	compiler
FROM eagilog.stream
LEFT JOIN eagilog.stream_command USING(stream_command_id)
WHERE finish_time IS NOT NULL;

CREATE VIEW eagilog.active_stream
AS
SELECT
	stream_id, application_id,
	start_time,
	current_timestamp - start_time AS duration,
	command,
	git_hash,
	git_version,
	os_name,
	hostname,
	architecture,
	compiler
FROM eagilog.stream
LEFT JOIN eagilog.stream_command USING(stream_command_id)
WHERE finish_time IS NULL;

CREATE VIEW eagilog.any_stream
AS
SELECT
	stream_id, application_id,
	start_time, finish_time,
	coalesce(finish_time, current_timestamp) - start_time AS duration,
	command,
	git_hash,
	git_version,
	os_name,
	hostname,
	architecture,
	compiler
FROM eagilog.stream
LEFT JOIN eagilog.stream_command USING(stream_command_id);
--------------------------------------------------------------------------------
-- entry
--------------------------------------------------------------------------------
CREATE TABLE eagilog.entry(
	entry_id BIGSERIAL PRIMARY KEY,
	stream_id INTEGER NOT NULL,
	source_id VARCHAR(10) NOT NULL,
	instance BIGINT NOT NULL,
	severity_id SMALLINT NOT NULL,
	tag VARCHAR(10) NULL,
	message_format_id INTEGER NOT NULL,
	entry_time INTERVAL NOT NULL
);

ALTER TABLE eagilog.entry
ADD FOREIGN KEY(stream_id)
REFERENCES eagilog.stream;

ALTER TABLE eagilog.entry
ADD FOREIGN KEY(message_format_id)
REFERENCES eagilog.message_format;

CREATE FUNCTION eagilog.add_entry(
	_stream_id eagilog.entry.stream_id%TYPE,
	_source_id eagilog.entry.source_id%TYPE,
	_instance eagilog.entry.instance%TYPE,
	_severity_name eagilog.severity.name%TYPE,
	_tag eagilog.entry.tag%TYPE,
	_format eagilog.message_format.format%TYPE
) RETURNS eagilog.entry.entry_id%TYPE
AS $$
DECLARE
	_entry_time INTERVAL;
	result eagilog.entry.entry_id%TYPE;
BEGIN
	SELECT now() - start_time
	INTO _entry_time
	FROM eagilog.stream
	WHERE stream_id = _stream_id;

	WITH ins AS (
		INSERT INTO eagilog.entry (
			stream_id,
			source_id,
			instance,
			severity_id,
			tag,
			message_format_id,
			entry_time
		) VALUES (
			_stream_id,
			_source_id,
			_instance,
			eagilog.get_severity_id(_severity_name),
            _tag,
			eagilog.get_message_format_id(_format),
			_entry_time
		) RETURNING eagilog.entry.entry_id
	)
	SELECT entry_id INTO result FROM ins;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.add_entry_tag(
	_entry_id eagilog.entry.entry_id%TYPE,
	_tag eagilog.entry.tag%TYPE
) RETURNS eagilog.entry.entry_id%TYPE
AS $$
BEGIN
	UPDATE eagilog.entry
	SET tag = _tag
	WHERE entry_id = _entry_id;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- format / entry views
--------------------------------------------------------------------------------
CREATE VIEW eagilog.message_format_ref_count
AS
SELECT
    message_format_id, hash, format,
    count(entry_id) AS ref_count
FROM eagilog.message_format
JOIN eagilog.entry
USING (message_format_id)
GROUP BY (message_format_id, hash, format);
--------------------------------------------------------------------------------
-- arg_string
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_string(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_type VARCHAR(10) NULL,
	value VARCHAR(80) NOT NULL
);

ALTER TABLE eagilog.arg_string
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_string
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry;

CREATE FUNCTION eagilog.add_entry_arg_string(
	_entry_id eagilog.arg_string.entry_id%TYPE,
	_arg_id eagilog.arg_string.arg_id%TYPE,
	_type eagilog.arg_string.arg_type%TYPE,
	_value eagilog.arg_string.value%TYPE
) RETURNS VOID
AS $$
BEGIN
	INSERT INTO eagilog.arg_string
	(entry_id, arg_id, arg_type, value)
	VALUES(_entry_id, _arg_id, _type, _value)
	ON CONFLICT ON CONSTRAINT arg_string_pkey
	DO NOTHING;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_boolean
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_boolean(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_type VARCHAR(10) NULL,
	value BOOL NOT NULL
);

ALTER TABLE eagilog.arg_boolean
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_boolean
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry;

CREATE FUNCTION eagilog.add_entry_arg_boolean(
	_entry_id eagilog.arg_boolean.entry_id%TYPE,
	_arg_id eagilog.arg_boolean.arg_id%TYPE,
	_type eagilog.arg_boolean.arg_type%TYPE,
	_value eagilog.arg_boolean.value%TYPE
) RETURNS VOID
AS $$
BEGIN
	INSERT INTO eagilog.arg_boolean
	(entry_id, arg_id, arg_type, value)
	VALUES(_entry_id, _arg_id, _type, _value)
	ON CONFLICT ON CONSTRAINT arg_boolean_pkey
	DO NOTHING;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_integer
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_integer(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_type VARCHAR(10) NULL,
	value BIGINT NOT NULL
);

ALTER TABLE eagilog.arg_integer
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_integer
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry;

CREATE FUNCTION eagilog.add_entry_arg_integer(
	_entry_id eagilog.arg_integer.entry_id%TYPE,
	_arg_id eagilog.arg_integer.arg_id%TYPE,
	_type eagilog.arg_integer.arg_type%TYPE,
	_value eagilog.arg_integer.value%TYPE
) RETURNS VOID
AS $$
BEGIN
	INSERT INTO eagilog.arg_integer
	(entry_id, arg_id, arg_type, value)
	VALUES(_entry_id, _arg_id, _type, _value)
	ON CONFLICT ON CONSTRAINT arg_integer_pkey
	DO NOTHING;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_float
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_float(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_type VARCHAR(10) NULL,
	value DOUBLE PRECISION NOT NULL
);

ALTER TABLE eagilog.arg_float
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_float
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry;

CREATE FUNCTION eagilog.add_entry_arg_float(
	_entry_id eagilog.arg_float.entry_id%TYPE,
	_arg_id eagilog.arg_float.arg_id%TYPE,
	_type eagilog.arg_float.arg_type%TYPE,
	_value eagilog.arg_float.value%TYPE
) RETURNS VOID
AS $$
BEGIN
	INSERT INTO eagilog.arg_float
	(entry_id, arg_id, arg_type, value)
	VALUES(_entry_id, _arg_id, _type, _value)
	ON CONFLICT ON CONSTRAINT arg_float_pkey
	DO NOTHING;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_duration
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_duration(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_type VARCHAR(10) NULL,
	value INTERVAL NOT NULL
);

ALTER TABLE eagilog.arg_duration
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_duration
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry;

CREATE FUNCTION eagilog.add_entry_arg_duration(
	_entry_id eagilog.arg_duration.entry_id%TYPE,
	_arg_id eagilog.arg_duration.arg_id%TYPE,
	_type eagilog.arg_duration.arg_type%TYPE,
	_value eagilog.arg_duration.value%TYPE
) RETURNS VOID
AS $$
BEGIN
	INSERT INTO eagilog.arg_duration
	(entry_id, arg_id, arg_type, value)
	VALUES(_entry_id, _arg_id, _type, _value)
	ON CONFLICT ON CONSTRAINT arg_duration_pkey
	DO NOTHING;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- profiling intervals
--------------------------------------------------------------------------------
CREATE TABLE eagilog.profile_interval(
	interval_id BIGSERIAL PRIMARY KEY,
	stream_id INTEGER NOT NULL,
	source_id VARCHAR(10) NOT NULL,
	tag VARCHAR(10) NOT NULL,
	hit_count BIGINT NOT NULL,
	hit_interval INTERVAL NOT NULL,
	min_duration_ms REAL NOT NULL,
	avg_duration_ms REAL NOT NULL,
	max_duration_ms REAL NOT NULL,
	entry_time INTERVAL NOT NULL
);

ALTER TABLE eagilog.profile_interval
ADD FOREIGN KEY(stream_id)
REFERENCES eagilog.stream;

CREATE FUNCTION eagilog.add_profile_interval(
	_stream_id eagilog.profile_interval.stream_id%TYPE,
	_source_id eagilog.profile_interval.source_id%TYPE,
	_tag eagilog.profile_interval.tag%TYPE,
	_hit_count eagilog.profile_interval.hit_count%TYPE,
	_hit_interval eagilog.profile_interval.hit_interval%TYPE,
	_min_duration_ms eagilog.profile_interval.min_duration_ms%TYPE,
	_avg_duration_ms eagilog.profile_interval.avg_duration_ms%TYPE,
	_max_duration_ms eagilog.profile_interval.max_duration_ms%TYPE
) RETURNS eagilog.profile_interval.interval_id%TYPE
AS $$
DECLARE
	_entry_time INTERVAL;
	result eagilog.profile_interval.interval_id%TYPE;
BEGIN
	SELECT now() - start_time
	INTO _entry_time
	FROM eagilog.stream
	WHERE stream_id = _stream_id;

	WITH ins AS (
		INSERT INTO eagilog.profile_interval (
			stream_id,
			source_id,
			tag,
			hit_count,
			hit_interval,
			min_duration_ms,
			avg_duration_ms,
			max_duration_ms,
			entry_time
		) VALUES (
			_stream_id,
			_source_id,
			_tag,
			_hit_count,
			_hit_interval,
			_min_duration_ms,
			_avg_duration_ms,
			_max_duration_ms,
			_entry_time
		) RETURNING eagilog.profile_interval.interval_id
	)
	SELECT interval_id INTO result FROM ins;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE VIEW eagilog.stream_profile_intervals
AS
SELECT
	i.stream_id,
	s.application_id,
	i.source_id,
	i.tag,
	sum(i.hit_count) AS hit_count,
	sum(i.hit_interval) AS hit_interval,
	min(i.min_duration_ms) AS min_duration_ms,
	avg(i.avg_duration_ms) AS avg_duration_ms,
	max(i.max_duration_ms) AS max_duration_ms,
	s.start_time,
	s.finish_time
FROM eagilog.profile_interval i
JOIN eagilog.stream s USING(stream_id)
GROUP BY i.stream_id, s.application_id, i.source_id, i.tag, s.start_time, s.finish_time;
--------------------------------------------------------------------------------
-- other views
--------------------------------------------------------------------------------
-- count of entries by source
--------------------------------------------------------------------------------
CREATE VIEW eagilog.source_entry_counts
AS
SELECT
	source_id,
	count(entry_id) source_entry_count
FROM eagilog.entry
GROUP BY source_id;
--------------------------------------------------------------------------------
-- count of entries with specific source/tag combination
--------------------------------------------------------------------------------
CREATE VIEW eagilog.message_entry_counts
AS
SELECT
	source_id, tag,
	count(entry_id) message_entry_count
FROM eagilog.entry
GROUP BY source_id, tag;
--------------------------------------------------------------------------------
-- various numeric values merged into one view
--------------------------------------------------------------------------------
CREATE VIEW eagilog.numeric_streams
AS
SELECT
	stream_id,
	s.application_id,
	source_id || '.' || tag || '.' || arg_id AS value_id,
	s.start_time,
	s.finish_time,
	s.start_time + e.entry_time AS value_time,
	a.arg_type AS value_type,
	a.value
FROM eagilog.any_stream s
JOIN eagilog.entry e USING(stream_id)
JOIN eagilog.arg_float a USING(entry_id)
WHERE e.tag IS NOT NULL
UNION
SELECT
	stream_id,
	s.application_id,
	source_id || '.' || tag || '.' || arg_id AS value_id,
	s.start_time,
	s.finish_time,
	s.start_time + e.entry_time AS value_time,
	a.arg_type AS value_type,
	a.value::DOUBLE PRECISION
FROM eagilog.any_stream s
JOIN eagilog.entry e USING(stream_id)
JOIN eagilog.arg_integer a USING(entry_id)
WHERE e.tag IS NOT NULL;
--------------------------------------------------------------------------------
-- various time-duration values merged into one view
--------------------------------------------------------------------------------
CREATE VIEW eagilog.duration_streams
AS
SELECT
	stream_id,
	s.application_id,
	source_id || '.' || tag || '.' || arg_id AS value_id,
	s.start_time,
	s.finish_time,
	s.start_time + e.entry_time AS value_time,
	a.arg_type AS value_type,
	value
FROM eagilog.any_stream s
JOIN eagilog.entry e USING(stream_id)
JOIN eagilog.arg_duration a USING(entry_id)
WHERE e.tag IS NOT NULL
--------------------------------------------------------------------------------
-- END
--------------------------------------------------------------------------------

