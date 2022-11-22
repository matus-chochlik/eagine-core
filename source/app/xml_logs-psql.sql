DROP SCHEMA eagilog CASCADE;

CREATE SCHEMA eagilog;

--------------------------------------------------------------------------------
-- message_format 
--------------------------------------------------------------------------------
CREATE TABLE eagilog.message_format (
	message_format_id SERIAL PRIMARY KEY,
	hash BYTEA NOT NULL,
	format VARCHAR(160) NOT NULL
);

CREATE UNIQUE INDEX hash_index
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
-- stream
--------------------------------------------------------------------------------
CREATE TABLE eagilog.stream (
	stream_id SERIAL PRIMARY KEY,
	start_time TIMESTAMP WITH TIME ZONE NOT NULL,
	finish_time TIMESTAMP WITH TIME ZONE
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
-- entry
--------------------------------------------------------------------------------
CREATE TABLE eagilog.entry(
	entry_id BIGSERIAL PRIMARY KEY,
	stream_id INTEGER NOT NULL,
	source_id VARCHAR(10) NOT NULL,
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
		INSERT INTO eagilog.entry (stream_id, source_id, message_format_id, entry_time)
		VALUES(_stream_id, _source_id, eagilog.get_message_format_id(_format), _entry_time)
		RETURNING eagilog.entry.entry_id
	)
	SELECT entry_id INTO result FROM ins;
	RETURN result;
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- END
--------------------------------------------------------------------------------
