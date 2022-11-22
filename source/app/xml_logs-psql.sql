DROP SCHEMA eagine CASCADE;

CREATE SCHEMA eagine;

--------------------------------------------------------------------------------
-- log_message_format 
--------------------------------------------------------------------------------
CREATE TABLE eagine.log_message_format (
	log_message_format_id SERIAL PRIMARY KEY,
	hash BYTEA NOT NULL,
	format VARCHAR(160) NOT NULL
);

CREATE UNIQUE INDEX hash_index
ON eagine.log_message_format (hash);

CREATE FUNCTION eagine.trigger_log_message_format_hash_default()
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
BEFORE INSERT OR UPDATE ON eagine.log_message_format
FOR EACH ROW EXECUTE PROCEDURE eagine.trigger_log_message_format_hash_default();

CREATE FUNCTION eagine.get_log_message_format_id(
	eagine.log_message_format.hash%TYPE,
	eagine.log_message_format.format%TYPE
) RETURNS eagine.log_message_format.log_message_format_id%TYPE
AS $$
DECLARE
	result eagine.log_message_format.log_message_format_id%TYPE;
BEGIN
	LOOP
		BEGIN
			WITH sel AS (
				SELECT lmf.log_message_format_id
				FROM eagine.log_message_format lmf
				WHERE lmf.hash = $1
				FOR SHARE
			)  , ins AS (
				INSERT INTO eagine.log_message_format (format)
				SELECT $2
				WHERE NOT EXISTS (SELECT 1 FROM sel)
				RETURNING eagine.log_message_format.log_message_format_id
			)
			SELECT sel.log_message_format_id FROM sel
			UNION ALL
			SELECT ins.log_message_format_id FROM ins
			INTO result;
		EXCEPTION WHEN UNIQUE_VIOLATION THEN
			result := NULL;
		END;

		EXIT WHEN result IS NOT NULL;
	END LOOP;
	RETURN result;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION eagine.get_log_message_format_id(
	eagine.log_message_format.format%TYPE
) RETURNS eagine.log_message_format.log_message_format_id%TYPE
AS $$
BEGIN
	RETURN eagine.get_log_message_format_id(decode(md5($1), 'hex'), $1);
END
$$ LANGUAGE plpgsql;
--------------------------------------------------------------------------------
-- END
--------------------------------------------------------------------------------
