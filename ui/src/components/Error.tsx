import { ExclamationCircleIcon } from '@heroicons/react/24/outline';
import { useTranslation } from 'react-i18next';
import { version } from '../../package.json';
import { Button } from './Button';
import { Log } from './Log';

type ErrorContentProps = { error: unknown };

const ErrorContent = ({ error }: ErrorContentProps) => {
  const { t } = useTranslation();

  return (
    <>
      <ExclamationCircleIcon className="mb-2 h-12 w-12 text-red-700" />
      <h3 className="text-2xl">{t('error.headline')}</h3>
      <h4 className="text-xl">{t('error.desc')}</h4>
      <Log className="mt-6">{(error as Error)?.message ?? error}</Log>
    </>
  );
};

type ErrorProps = { error: unknown; resetErrorBoundary?: () => void };

export const Error = ({
  error,
  resetErrorBoundary,
}: ErrorProps): JSX.Element => {
  const { t } = useTranslation();

  if (resetErrorBoundary) {
    return (
      <div className="flex flex-col items-center">
        <ErrorContent error={error} />
        <Button className="mt-6" onClick={() => resetErrorBoundary()}>
          {t('error.retry')}
        </Button>
      </div>
    );
  }

  return (
    <>
      <h1 className="text-3xl">NikoLight</h1>
      <h2 className="text-base text-zinc">v{version}</h2>
      <div className="mt-12 flex flex-col items-center">
        <ErrorContent error={error} />
      </div>
    </>
  );
};
