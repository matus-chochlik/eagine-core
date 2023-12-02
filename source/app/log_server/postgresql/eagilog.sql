DROP SCHEMA eagilog CASCADE;

CREATE SCHEMA eagilog;

--------------------------------------------------------------------------------
-- statistics
--------------------------------------------------------------------------------
CREATE VIEW eagilog.schema_statistics
AS
SELECT
	relname AS table_name,
	reltuples AS row_count,
	pg_total_relation_size(c.oid) AS total_bytes,
	pg_size_pretty(pg_total_relation_size(c.oid)) AS pretty_size
FROM pg_class c
LEFT JOIN pg_namespace n ON n.oid = c.relnamespace
WHERE relkind = 'r'
AND nspname = 'eagilog';

CREATE FUNCTION eagilog.schema_data_size_kiB() RETURNS NUMERIC
AS 'SELECT round(sum(total_bytes) / 1024, 3)::NUMERIC FROM eagilog.schema_statistics;'
LANGUAGE sql IMMUTABLE;

CREATE FUNCTION eagilog.schema_data_size_MiB() RETURNS NUMERIC
AS 'SELECT round(sum(total_bytes) / 1048576, 3)::NUMERIC FROM eagilog.schema_statistics;'
LANGUAGE sql IMMUTABLE;

CREATE FUNCTION eagilog.schema_data_size_GiB() RETURNS NUMERIC
AS 'SELECT round(sum(total_bytes) / 1073741824, 3)::NUMERIC FROM eagilog.schema_statistics;'
LANGUAGE sql IMMUTABLE;
--------------------------------------------------------------------------------
-- client session handling
--------------------------------------------------------------------------------
CREATE TABLE eagilog.client_session (
	client_session_id BIGSERIAL PRIMARY KEY,
	backend_pid INTEGER NOT NULL,
	heartbeat_time TIMESTAMP WITH TIME ZONE NOT NULL
);
--------------------------------------------------------------------------------
-- starts a new session or gets the current session for current connection
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.get_client_session()
RETURNS eagilog.client_session.client_session_id%TYPE
AS $$
DECLARE
	result eagilog.client_session.client_session_id%TYPE;
BEGIN
	LOOP
		BEGIN
			WITH sel AS (
				SELECT cs.client_session_id
				FROM eagilog.client_session cs
				WHERE cs.backend_pid = pg_backend_pid()
				FOR SHARE
			)  , ins AS (
				INSERT INTO eagilog.client_session (backend_pid, heartbeat_time)
				SELECT pg_backend_pid(), now()
				WHERE NOT EXISTS (SELECT 1 FROM sel)
				RETURNING eagilog.client_session.client_session_id
			)
			SELECT sel.client_session_id FROM sel
			UNION ALL
			SELECT ins.client_session_id FROM ins
			INTO result;
		EXCEPTION WHEN UNIQUE_VIOLATION THEN
			result := NULL;
		END;

		EXIT WHEN result IS NOT NULL;
	END LOOP;
	RETURN result;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.client_session_heartbeat()
RETURNS VOID
AS
$$
UPDATE eagilog.client_session
SET heartbeat_time = now()
WHERE backend_pid = pg_backend_pid()
$$
LANGUAGE sql;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.finish_client_session()
RETURNS VOID
AS
$$
DELETE FROM eagilog.client_session
WHERE backend_pid = pg_backend_pid();
$$
LANGUAGE sql;
--------------------------------------------------------------------------------
-- entry tags blocked by client
--------------------------------------------------------------------------------
CREATE TABLE eagilog.client_session_blocked_entry_message (
	client_session_id BIGINT NOT NULL,
	application_id VARCHAR(10) NOT NULL,
	source_id VARCHAR(10) NOT NULL,
	tag VARCHAR(10) NOT NULL
);

ALTER TABLE eagilog.client_session_blocked_entry_message
ADD PRIMARY KEY(client_session_id, application_id, source_id, tag);

ALTER TABLE eagilog.client_session_blocked_entry_message
ADD FOREIGN KEY(client_session_id)
REFERENCES eagilog.client_session
ON DELETE CASCADE;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.client_session_block_entry_message(
	_application_id VARCHAR(10),
	_source_id VARCHAR(10),
	_tag VARCHAR(10)
) RETURNS VOID
AS
$$
	INSERT INTO eagilog.client_session_blocked_entry_message VALUES(
		eagilog.get_client_session(),
		coalesce(_application_id, ''),
		coalesce(_source_id, ''),
		coalesce(_tag, ''));
$$ LANGUAGE sql;
--------------------------------------------------------------------------------
CREATE VIEW eagilog.client_blocked_entry_messages
AS
SELECT application_id, source_id, tag
FROM eagilog.client_session
JOIN eagilog.client_session_blocked_entry_message USING(client_session_id)
WHERE backend_pid = pg_backend_pid();
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION eagilog.is_blocked_client_message(
	_application_id VARCHAR(10),
	_source_id VARCHAR(10),
	_tag VARCHAR(10)
) RETURNS BOOLEAN
AS
$$
SELECT EXISTS(
	SELECT *
	FROM eagilog.client_blocked_entry_messages
	WHERE (application_id = _application_id OR application_id = '')
	AND (source_id = _source_id OR source_id = '')
	AND (tag = _tag OR tag = '')
);
$$ LANGUAGE sql IMMUTABLE;
--------------------------------------------------------------------------------
-- benchmark type
--------------------------------------------------------------------------------
CREATE TABLE eagilog.benchmark_type (
	benchmark_type_id SMALLINT PRIMARY KEY,
	name VARCHAR(16) NOT NULL
);

CREATE UNIQUE INDEX benchmark_type_name_index
ON eagilog.benchmark_type (name);

CREATE FUNCTION eagilog.get_benchmark_type_id(
	eagilog.benchmark_type.name%TYPE
) RETURNS eagilog.benchmark_type.benchmark_type_id%TYPE
AS $$
DECLARE
	result eagilog.benchmark_type.benchmark_type_id%TYPE;
BEGIN
	SELECT benchmark_type_id
	INTO result
	FROM eagilog.benchmark_type
	WHERE name = $1;
	RETURN result;
END
$$ LANGUAGE plpgsql;

INSERT INTO eagilog.benchmark_type (benchmark_type_id, name) VALUES(0, 'build_time');
--------------------------------------------------------------------------------
-- benchmark
--------------------------------------------------------------------------------
CREATE TABLE eagilog.benchmark (
	benchmark_id SERIAL PRIMARY KEY,
	benchmark_type_id INTEGER NOT NULL,
	benchmark_time TIMESTAMP WITH TIME ZONE NOT NULL,
	duration INTERVAL NOT NULL,
	git_hash VARCHAR(64) NULL,
	git_version VARCHAR(32) NULL,
	os_name VARCHAR(64) NULL,
	hostname VARCHAR(64) NULL,
	architecture VARCHAR(32) NULL,
	compiler VARCHAR(32) NULL,
	low_profile_build BOOL NULL,
	debug_build BOOL NULL
);

ALTER TABLE eagilog.benchmark
ADD FOREIGN KEY(benchmark_type_id)
REFERENCES eagilog.benchmark_type
ON DELETE CASCADE;
--------------------------------------------------------------------------------
-- severity
--------------------------------------------------------------------------------
CREATE TABLE eagilog.severity (
	severity_id SMALLINT PRIMARY KEY,
	name VARCHAR(9) NOT NULL
);

CREATE UNIQUE INDEX severity_name_index
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
	RETURN result;
END
$$ LANGUAGE plpgsql;

INSERT INTO eagilog.severity (severity_id, name) VALUES(0, 'backtrace');
INSERT INTO eagilog.severity (severity_id, name) VALUES(1, 'trace');
INSERT INTO eagilog.severity (severity_id, name) VALUES(2, 'debug');
INSERT INTO eagilog.severity (severity_id, name) VALUES(3, 'stat');
INSERT INTO eagilog.severity (severity_id, name) VALUES(4, 'info');
INSERT INTO eagilog.severity (severity_id, name) VALUES(5, 'change');
INSERT INTO eagilog.severity (severity_id, name) VALUES(6, 'warning');
INSERT INTO eagilog.severity (severity_id, name) VALUES(7, 'error');
INSERT INTO eagilog.severity (severity_id, name) VALUES(8, 'fatal');
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
	first_entry_id BIGINT NULL,
	last_entry_id BIGINT NULL,
	stream_command_id INTEGER NULL,
	os_pid INTEGER NULL,
	endpoint_id INTEGER NULL,
	git_hash VARCHAR(64) NULL,
	git_version VARCHAR(32) NULL,
	os_name VARCHAR(64) NULL,
	hostname VARCHAR(64) NULL,
	architecture VARCHAR(32) NULL,
	compiler VARCHAR(32) NULL,
	running_on_valgrind BOOL NULL,
	low_profile_build BOOL NULL,
	debug_build BOOL NULL,
	failed BOOL NOT NULL DEFAULT FALSE
);
--------------------------------------------------------------------------------
-- declared stream states
--------------------------------------------------------------------------------
CREATE TABLE eagilog.declared_stream_state(
	stream_id INTEGER NOT NULL,
	source_id VARCHAR(10) NOT NULL,
	status_tag VARCHAR(10) NOT NULL,
	begin_tag VARCHAR(10) NULL,
	end_tag VARCHAR(10) NULL,
	is_active BOOL NOT NULL DEFAULT FALSE
);

ALTER TABLE eagilog.declared_stream_state
ADD PRIMARY KEY(stream_id, source_id, status_tag);

ALTER TABLE eagilog.declared_stream_state
ADD FOREIGN KEY(stream_id)
REFERENCES eagilog.stream
ON DELETE CASCADE;

CREATE FUNCTION eagilog.declare_stream_state(
	_stream_id eagilog.declared_stream_state.stream_id%TYPE,
	_source_id eagilog.declared_stream_state.source_id%TYPE,
	_status_tag eagilog.declared_stream_state.status_tag%TYPE,
	_begin_tag eagilog.declared_stream_state.begin_tag%TYPE,
	_end_tag eagilog.declared_stream_state.end_tag%TYPE
) RETURNS eagilog.declared_stream_state.status_tag%TYPE
AS $$
DECLARE
	result eagilog.declared_stream_state.status_tag%TYPE;
BEGIN
	BEGIN
		INSERT INTO eagilog.declared_stream_state (
			stream_id,
			source_id,
			status_tag,
			begin_tag,
			end_tag
		) VALUES (
			_stream_id,
			_source_id,
			_status_tag,
			_begin_tag,
			_end_tag
		);
	EXCEPTION WHEN UNIQUE_VIOLATION THEN
		UPDATE eagilog.declared_stream_state
		SET begin_tag = _begin_tag, end_tag = _end_tag
		WHERE stream_id = _stream_id
		AND source_id = _source_id
		AND status_tag = _status_tag;
	END;
	RETURN _status_tag;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.make_stream_state_active(
	_stream_id eagilog.declared_stream_state.stream_id%TYPE,
	_source_id eagilog.declared_stream_state.source_id%TYPE,
	_status_tag eagilog.declared_stream_state.status_tag%TYPE
) RETURNS eagilog.declared_stream_state.status_tag%TYPE
AS $$
BEGIN
	BEGIN
		INSERT INTO eagilog.declared_stream_state (
			stream_id,
			source_id,
			status_tag
		) VALUES (
			_stream_id,
			_source_id,
			_status_tag
		);
	EXCEPTION WHEN UNIQUE_VIOLATION THEN
		UPDATE eagilog.declared_stream_state
		SET is_active = TRUE
		WHERE stream_id = _stream_id
		AND source_id = _source_id
		AND status_tag = _status_tag;
	END;
	RETURN _status_tag;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- stream states
--------------------------------------------------------------------------------
CREATE TABLE eagilog.stream_state(
	stream_id INTEGER NOT NULL,
	begin_entry_id BIGINT NOT NULL,
	end_entry_id BIGINT NULL,
	source_id VARCHAR(10) NOT NULL,
	instance BIGINT NOT NULL,
	status_tag VARCHAR(10) NOT NULL,
	begin_time TIMESTAMP WITH TIME ZONE NOT NULL,
	end_time TIMESTAMP WITH TIME ZONE NULL,
	is_active BOOL NOT NULL
);

ALTER TABLE eagilog.stream_state
ADD PRIMARY KEY(stream_id, source_id, instance, status_tag, begin_time);

ALTER TABLE eagilog.stream_state
ADD FOREIGN KEY(stream_id)
REFERENCES eagilog.stream
ON DELETE CASCADE;
--------------------------------------------------------------------------------
CREATE VIEW eagilog.any_stream_state
AS
SELECT
	stream_id,
	begin_entry_id,
	end_entry_id,
	source_id,
	instance,
	status_tag,
	begin_time,
	end_time,
	coalesce(end_time, current_timestamp) - begin_time AS duration,
	end_time IS NOT NULL AS has_finished,
	is_active
FROM eagilog.stream_state;
--------------------------------------------------------------------------------
-- start / end stream
--------------------------------------------------------------------------------
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
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.finish_stream(
	_stream_id eagilog.stream.stream_id%TYPE,
	_clean_shutdown BOOL
) RETURNS eagilog.stream.stream_id%TYPE
AS $$
BEGIN
	UPDATE eagilog.stream
	SET finish_time = start_time + (
		SELECT max(entry_time)
		FROM eagilog.entry
		WHERE stream_id = _stream_id
	), last_entry_id = (
		SELECT max(entry_id)
		FROM eagilog.entry
		WHERE stream_id = _stream_id
	), failed = NOT _clean_shutdown
	WHERE stream_id = _stream_id;

	UPDATE eagilog.stream_state
	SET end_time = now()
	WHERE stream_id = _stream_id
	AND end_time is NULL;

	DELETE FROM eagilog.declared_stream_state
	WHERE stream_id = _stream_id;

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
CREATE FUNCTION eagilog.set_stream_os_pid(
	_stream_id eagilog.stream.os_pid%TYPE,
	_value INTEGER
) RETURNS VOID
AS $$
BEGIN
	UPDATE eagilog.stream
	SET os_pid = _value
	WHERE stream_id = _stream_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagilog.set_stream_endpoint_id(
	_stream_id eagilog.stream.os_pid%TYPE,
	_value INTEGER
) RETURNS VOID
AS $$
BEGIN
	IF _value IS NOT NULL
	THEN
		UPDATE eagilog.stream
		SET endpoint_id = CASE
			WHEN endpoint_id IS NULL THEN _value
			WHEN endpoint_id = _value THEN _value
			ELSE NULL
		END
		WHERE stream_id = _stream_id;
	END IF;
END
$$ LANGUAGE plpgsql;

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

CREATE FUNCTION eagilog.set_stream_running_on_valgrind(
	_stream_id eagilog.stream.stream_id%TYPE,
	_value VARCHAR
) RETURNS VOID
AS $$
BEGIN
	IF _value = 'yes'
	THEN
		UPDATE eagilog.stream
		SET running_on_valgrind = TRUE
		WHERE stream_id = _stream_id;
	ELSIF _value = 'no'
	THEN
		UPDATE eagilog.stream
		SET running_on_valgrind = FALSE
		WHERE stream_id = _stream_id;
	END IF;
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
-- other stream functions
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.contemporary_streams(
	_when TIMESTAMP WITH TIME ZONE
) RETURNS INTEGER[]
AS $$
DECLARE
	result INTEGER[];
BEGIN
	SELECT array(
		SELECT stream_id
		FROM eagilog.stream
		WHERE start_time <= _when
		AND  _when <= coalesce(finish_time, _when)
		ORDER BY stream_id
	) INTO result;
	RETURN result;
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
	compiler,
	running_on_valgrind,
	low_profile_build,
	debug_build
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
	compiler,
	running_on_valgrind,
	low_profile_build,
	debug_build
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
	compiler,
	running_on_valgrind,
	low_profile_build,
	debug_build
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
REFERENCES eagilog.stream
ON DELETE CASCADE;

ALTER TABLE eagilog.entry
ADD FOREIGN KEY(message_format_id)
REFERENCES eagilog.message_format
ON DELETE CASCADE;

ALTER TABLE eagilog.stream_state
ADD FOREIGN KEY(begin_entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

ALTER TABLE eagilog.stream_state
ADD FOREIGN KEY(end_entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry(
	_stream_id eagilog.entry.stream_id%TYPE,
	_source_id eagilog.entry.source_id%TYPE,
	_instance eagilog.entry.instance%TYPE,
	_severity_name eagilog.severity.name%TYPE,
	_tag eagilog.entry.tag%TYPE,
	_format eagilog.message_format.format%TYPE,
	_entry_time eagilog.entry.entry_time%TYPE
) RETURNS eagilog.entry.entry_id%TYPE
AS $$
DECLARE
	result eagilog.entry.entry_id%TYPE;
BEGIN

	IF _entry_time IS NULL
	THEN
		SELECT now() - start_time
		INTO _entry_time
		FROM eagilog.stream
		WHERE stream_id = _stream_id;
	END IF;

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

	BEGIN
		INSERT INTO eagilog.stream_state (
			stream_id,
			begin_entry_id,
			source_id,
			instance,
			status_tag,
			is_active,
			begin_time
		) SELECT
			stream_id,
			result,
			source_id,
			_instance AS instance,
			status_tag,
			is_active,
			now() AS begin_time
		FROM eagilog.declared_stream_state
		WHERE stream_id = _stream_id
		AND source_id = _source_id
		AND begin_tag = _tag;
	EXCEPTION WHEN UNIQUE_VIOLATION THEN
	END;

	UPDATE eagilog.stream_state
	SET end_entry_id = result, end_time = now()
	WHERE end_time IS NOT NULL
	AND EXISTS (
		SELECT 1
		FROM eagilog.declared_stream_state
		WHERE eagilog.declared_stream_state.stream_id = eagilog.stream_state.stream_id
		AND eagilog.declared_stream_state.source_id = eagilog.stream_state.source_id
		AND eagilog.declared_stream_state.status_tag = eagilog.stream_state.status_tag
		AND eagilog.declared_stream_state.end_tag = _tag
	);

	UPDATE eagilog.stream
	SET first_entry_id = result
	WHERE stream_id = _stream_id
	AND first_entry_id IS NULL;

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
-- returns the latest overlapping set of streams
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.recent_streams()
RETURNS SETOF eagilog.stream
AS
$$
DECLARE
	_start_time TIMESTAMP WITH TIME ZONE;
	_finish_time TIMESTAMP WITH TIME ZONE;
	_row eagilog.stream;
BEGIN
	SELECT start_time, coalesce(finish_time, current_timestamp)
	INTO _start_time, _finish_time
	FROM eagilog.entry
	JOIN eagilog.stream USING(stream_id)
	ORDER BY entry_id DESC
	LIMIT 1;

	FOR _row IN
		SELECT *
		FROM eagilog.stream
		WHERE _start_time BETWEEN start_time AND coalesce(finish_time, current_timestamp)
		OR   _finish_time BETWEEN start_time AND coalesce(finish_time, current_timestamp)
	LOOP
		RETURN NEXT _row;
	END LOOP;
END;
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- format / entry views
--------------------------------------------------------------------------------
CREATE VIEW eagilog.stream_entry
AS
SELECT
	stream_id,
	e.entry_id,
	l.name AS severity,
	s.application_id,
	e.source_id,
	e.instance,
	e.tag,
	f.format,
	s.start_time + e.entry_time AS entry_time,
	entry_time AS time_since_start,
	s.first_entry_id = e.entry_id AS is_first,
	coalesce(s.last_entry_id = e.entry_id, FALSE) AS is_last,
	s.failed
FROM eagilog.entry e
JOIN eagilog.stream s USING(stream_id)
JOIN eagilog.severity l USING(severity_id)
JOIN eagilog.message_format f USING(message_format_id)
ORDER BY entry_id;
--------------------------------------------------------------------------------
CREATE VIEW eagilog.recent_stream_entry
AS
SELECT
	stream_id,
	e.entry_id,
	l.name AS severity,
	s.application_id,
	e.source_id,
	e.instance,
	e.tag,
	f.format,
	s.start_time + e.entry_time AS entry_time,
	entry_time AS time_since_start,
	s.first_entry_id = e.entry_id AS is_first,
	coalesce(s.last_entry_id = e.entry_id, FALSE) AS is_last,
	s.failed
FROM eagilog.entry e
JOIN eagilog.recent_streams() s USING(stream_id)
JOIN eagilog.severity l USING(severity_id)
JOIN eagilog.message_format f USING(message_format_id)
ORDER BY entry_id;
--------------------------------------------------------------------------------
-- all streams and entries
--------------------------------------------------------------------------------
CREATE VIEW eagilog.streams_and_entries
AS
SELECT eagilog.contemporary_streams(entry_time), *
FROM eagilog.stream_entry
ORDER BY entry_id DESC;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.latest_stream_entries(_count INTEGER)
RETURNS SETOF eagilog.streams_and_entries
AS
$$
SELECT *
FROM (SELECT * FROM eagilog.streams_and_entries LIMIT _count) AS tail
ORDER BY entry_id ASC;
$$ LANGUAGE sql;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.recent_stream_entries()
RETURNS SETOF eagilog.streams_and_entries
AS
$$
SELECT eagilog.contemporary_streams(entry_time), *
FROM eagilog.recent_stream_entry
ORDER BY entry_id;
$$ LANGUAGE sql;
--------------------------------------------------------------------------------
-- client streams and entries
--------------------------------------------------------------------------------
CREATE VIEW eagilog.client_streams_and_entries
AS
SELECT eagilog.contemporary_streams(entry_time), *
FROM eagilog.stream_entry
WHERE NOT eagilog.is_blocked_client_message(application_id, source_id, tag)
ORDER BY entry_id DESC;
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.latest_client_stream_entries(_count INTEGER)
RETURNS SETOF eagilog.client_streams_and_entries
AS
$$
SELECT *
FROM (SELECT * FROM eagilog.client_streams_and_entries LIMIT _count) AS tail
ORDER BY entry_id ASC;
$$ LANGUAGE sql;
--------------------------------------------------------------------------------
-- other format / entry views
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
	arg_order SMALLINT NOT NULL DEFAULT 0,
	arg_type VARCHAR(10) NULL,
	value VARCHAR(160) NOT NULL
);

ALTER TABLE eagilog.arg_string
ADD PRIMARY KEY(entry_id, arg_id, arg_order);

ALTER TABLE eagilog.arg_string
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_string(
	_entry_id eagilog.arg_string.entry_id%TYPE,
	_arg_id eagilog.arg_string.arg_id%TYPE,
	_type eagilog.arg_string.arg_type%TYPE,
	_value eagilog.arg_string.value%TYPE
) RETURNS VOID
AS $$
DECLARE
	_arg_order SMALLINT;
BEGIN
	SELECT count(1)
	INTO _arg_order
	FROM eagilog.arg_string
	WHERE entry_id = _entry_id
	AND arg_id = _arg_id;
	INSERT INTO eagilog.arg_string
	(entry_id, arg_id, arg_order, arg_type, value)
	VALUES(_entry_id, _arg_id, _arg_order, _type, _value);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_boolean
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_boolean(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_order SMALLINT NOT NULL DEFAULT 0,
	arg_type VARCHAR(10) NULL,
	value BOOL NOT NULL
);

ALTER TABLE eagilog.arg_boolean
ADD PRIMARY KEY(entry_id, arg_id, arg_order);

ALTER TABLE eagilog.arg_boolean
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_boolean(
	_entry_id eagilog.arg_boolean.entry_id%TYPE,
	_arg_id eagilog.arg_boolean.arg_id%TYPE,
	_type eagilog.arg_boolean.arg_type%TYPE,
	_value eagilog.arg_boolean.value%TYPE
) RETURNS VOID
AS $$
DECLARE
	_arg_order SMALLINT;
BEGIN
	SELECT count(1)
	INTO _arg_order
	FROM eagilog.arg_boolean
	WHERE entry_id = _entry_id
	AND arg_id = _arg_id;
	INSERT INTO eagilog.arg_boolean
	(entry_id, arg_id, arg_order, arg_type, value)
	VALUES(_entry_id, _arg_id, _arg_order, _type, _value);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_integer
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_integer(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_order SMALLINT NOT NULL DEFAULT 0,
	arg_type VARCHAR(10) NULL,
	value NUMERIC NOT NULL
);

ALTER TABLE eagilog.arg_integer
ADD PRIMARY KEY(entry_id, arg_id, arg_order);

ALTER TABLE eagilog.arg_integer
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_integer(
	_entry_id eagilog.arg_integer.entry_id%TYPE,
	_arg_id eagilog.arg_integer.arg_id%TYPE,
	_type eagilog.arg_integer.arg_type%TYPE,
	_value eagilog.arg_integer.value%TYPE
) RETURNS VOID
AS $$
DECLARE
	_arg_order SMALLINT;
BEGIN
	SELECT count(1)
	INTO _arg_order
	FROM eagilog.arg_integer
	WHERE entry_id = _entry_id
	AND arg_id = _arg_id;
	INSERT INTO eagilog.arg_integer
	(entry_id, arg_id, arg_order, arg_type, value)
	VALUES(_entry_id, _arg_id, _arg_order, _type, _value);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_float
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_float(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_order SMALLINT NOT NULL DEFAULT 0,
	arg_type VARCHAR(10) NULL,
	value DOUBLE PRECISION NOT NULL
);

ALTER TABLE eagilog.arg_float
ADD PRIMARY KEY(entry_id, arg_id, arg_order);

ALTER TABLE eagilog.arg_float
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_float(
	_entry_id eagilog.arg_float.entry_id%TYPE,
	_arg_id eagilog.arg_float.arg_id%TYPE,
	_type eagilog.arg_float.arg_type%TYPE,
	_value eagilog.arg_float.value%TYPE
) RETURNS VOID
AS $$
DECLARE
	_arg_order SMALLINT;
BEGIN
	SELECT count(1)
	INTO _arg_order
	FROM eagilog.arg_float
	WHERE entry_id = _entry_id
	AND arg_id = _arg_id;
	INSERT INTO eagilog.arg_float
	(entry_id, arg_id, arg_order, arg_type, value)
	VALUES(_entry_id, _arg_id, _arg_order, _type, _value);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_min_max(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	min_value DOUBLE PRECISION NOT NULL,
	max_value DOUBLE PRECISION NOT NULL
);

ALTER TABLE eagilog.arg_min_max
ADD PRIMARY KEY(entry_id, arg_id);

ALTER TABLE eagilog.arg_min_max
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_min_max(
	_entry_id eagilog.arg_float.entry_id%TYPE,
	_arg_id eagilog.arg_float.arg_id%TYPE,
	_min_value eagilog.arg_min_max.min_value%TYPE,
	_max_value eagilog.arg_min_max.max_value%TYPE
) RETURNS VOID
AS $$
BEGIN
	BEGIN
		INSERT INTO eagilog.arg_min_max
		(entry_id, arg_id, min_value, max_value)
		VALUES(_entry_id, _arg_id, _min_value, _max_value);
	EXCEPTION WHEN UNIQUE_VIOLATION THEN
		UPDATE eagilog.arg_min_max
		SET min_value = least(min_value, _min_value),
			max_value = greatest(max_value, _max_value)
		WHERE entry_id = _entry_id
		AND arg_id = _arg_id;
	END;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- arg_duration
--------------------------------------------------------------------------------
CREATE TABLE eagilog.arg_duration(
	entry_id BIGINT NOT NULL,
	arg_id VARCHAR(10) NOT NULL,
	arg_order SMALLINT NOT NULL DEFAULT 0,
	arg_type VARCHAR(10) NULL,
	value INTERVAL NOT NULL
);

ALTER TABLE eagilog.arg_duration
ADD PRIMARY KEY(entry_id, arg_id, arg_order);

ALTER TABLE eagilog.arg_duration
ADD FOREIGN KEY(entry_id)
REFERENCES eagilog.entry
ON DELETE CASCADE;

CREATE FUNCTION eagilog.add_entry_arg_duration(
	_entry_id eagilog.arg_duration.entry_id%TYPE,
	_arg_id eagilog.arg_duration.arg_id%TYPE,
	_type eagilog.arg_duration.arg_type%TYPE,
	_value eagilog.arg_duration.value%TYPE
) RETURNS VOID
AS $$
DECLARE
	_arg_order SMALLINT;
BEGIN
	SELECT count(1)
	INTO _arg_order
	FROM eagilog.arg_duration
	WHERE entry_id = _entry_id
	AND arg_id = _arg_id;
	INSERT INTO eagilog.arg_duration
	(entry_id, arg_id, arg_order, arg_type, value)
	VALUES(_entry_id, _arg_id, _arg_order, _type, _value);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- entry / argument views and functions
--------------------------------------------------------------------------------
CREATE VIEW eagilog.entry_and_args
AS
SELECT
	entry_id,
	arg_id,
	arg_type,
	arg_order,
	min_value,
	max_value,
	value AS value_integer,
	NULL::VARCHAR AS value_string,
	NULL::DOUBLE PRECISION AS value_float,
	NULL::INTERVAL AS value_duration,
	NULL::BOOL AS value_boolean
FROM arg_integer
LEFT JOIN arg_min_max USING(entry_id, arg_id)
UNION
SELECT
	entry_id,
	arg_id,
	arg_type,
	arg_order,
	NULL::DOUBLE PRECISION,
	NULL::DOUBLE PRECISION,
	NULL::NUMERIC,
	value,
	NULL::DOUBLE PRECISION,
	NULL::INTERVAL,
	NULL::BOOL
FROM arg_string
UNION
SELECT
	entry_id,
	arg_id,
	arg_type,
	arg_order,
	min_value,
	max_value,
	NULL::NUMERIC,
	NULL::VARCHAR,
	value,
	NULL::INTERVAL,
	NULL::BOOL
FROM arg_float
LEFT JOIN arg_min_max USING(entry_id, arg_id)
UNION
SELECT
	entry_id,
	arg_id,
	arg_type,
	arg_order,
	NULL::DOUBLE PRECISION,
	NULL::DOUBLE PRECISION,
	NULL::NUMERIC,
	NULL::VARCHAR,
	NULL::DOUBLE PRECISION,
	value,
	NULL::BOOL
FROM arg_duration
UNION
SELECT
	entry_id,
	arg_id,
	arg_type,
	arg_order,
	NULL::DOUBLE PRECISION,
	NULL::DOUBLE PRECISION,
	NULL::NUMERIC,
	NULL::VARCHAR,
	NULL::DOUBLE PRECISION,
	NULL::INTERVAL,
	value
FROM arg_boolean;
--------------------------------------------------------------------------------
-- argument of an entry
--------------------------------------------------------------------------------
CREATE FUNCTION eagilog.args_of_entry(
	_entry_id eagilog.entry.entry_id%TYPE
) RETURNS SETOF eagilog.entry_and_args AS
$$
	SELECT *
	FROM eagilog.entry_and_args
	WHERE entry_id = _entry_id
	AND arg_id IS NOT NULL;
$$ LANGUAGE sql;
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
REFERENCES eagilog.stream
ON DELETE CASCADE;

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
	s.finish_time,
	s.low_profile_build,
	s.debug_build
FROM eagilog.profile_interval i
JOIN eagilog.stream s USING(stream_id)
GROUP BY
	i.stream_id,
	s.application_id,
	i.source_id,
	i.tag,
	s.start_time,
	s.finish_time,
	s.low_profile_build,
	s.debug_build;
--------------------------------------------------------------------------------
-- other views
--------------------------------------------------------------------------------
-- count of entries by severity
--------------------------------------------------------------------------------
CREATE VIEW eagilog.severity_entry_counts
AS
SELECT
	s.name AS severity,
	count(entry_id) severity_entry_count
FROM eagilog.entry e
JOIN eagilog.severity s USING(severity_id)
GROUP BY severity_id, s.name;
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
	e.entry_time,
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
	e.entry_time,
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

