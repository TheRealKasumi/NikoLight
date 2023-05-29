import { saveAs } from 'file-saver';
import { useTranslation } from 'react-i18next';
import { Button, Log, Toast } from '../components';
import { toMbyte } from '../libs';
import { useClearLog, useLog, useLogSize } from './api';

export const LogFiles = (): JSX.Element => {
  const maxLogSize = 10000;

  const { t } = useTranslation();
  const { data: logSize } = useLogSize();

  const { startByte, byteCount } =
    logSize && logSize > maxLogSize
      ? { startByte: logSize - maxLogSize, byteCount: maxLogSize }
      : { startByte: 0, byteCount: logSize ?? 0 };

  const { data } = useLog({ startByte, byteCount });
  const { mutate, isLoading, isSuccess } = useClearLog();

  const onDownload = () => {
    // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
    const blob = new Blob([data!], { type: 'text/plain;charset=utf-8' });
    saveAs(blob, `NikoLight_${Date.now()}.log`);
  };

  const onClear = () => mutate();

  return (
    <>
      {isSuccess && <Toast title={t('logFiles.clearSuccessful')} />}

      <Log className="max-h-[65vh] overflow-scroll">{data}</Log>
      <p className="mb-6 text-right text-zinc">{toMbyte(logSize ?? 0)} MB</p>

      <Button
        type="button"
        className="mb-4"
        onClick={onDownload}
        disabled={isLoading}
      >
        {t('logFiles.download')}
      </Button>

      <Button type="reset" onClick={onClear} disabled={isLoading}>
        {t('logFiles.clear')}
      </Button>
    </>
  );
};
