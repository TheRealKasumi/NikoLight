import { ExclamationCircleIcon } from '@heroicons/react/24/outline';
import { version } from '../../package.json';
import { Button } from './Button';
import { Log } from './Log';

type ErrorContentProps = { error: unknown };

const ErrorContent = ({ error }: ErrorContentProps) => (
  <>
    <ExclamationCircleIcon className="mb-2 h-12 w-12 text-red-700" />
    <h3 className="text-2xl">Oops!</h3>
    <h4 className="text-xl">Something went wrong.</h4>
    <Log className="mt-6">{(error as Error)?.message ?? error}</Log>
  </>
);

type ErrorProps = { error: unknown; resetErrorBoundary?: () => void };

export const Error = ({ error, resetErrorBoundary }: ErrorProps): JSX.Element =>
  resetErrorBoundary ? (
    <div className="flex flex-col items-center">
      <ErrorContent error={error} />
      <Button className="mt-6" onClick={() => resetErrorBoundary()}>
        Try again
      </Button>
    </div>
  ) : (
    <>
      <h1 className="text-3xl">TesLight</h1>
      <h2 className="text-base text-zinc">v{version}</h2>
      <div className="mt-12 flex flex-col items-center">
        <ErrorContent error={error} />
      </div>
    </>
  );
