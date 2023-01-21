import { ExclamationCircleIcon } from '@heroicons/react/24/outline';
import { useTranslation } from 'react-i18next';
import { twMerge } from 'tailwind-merge';

type NotificationProps = {
  message: string;
  state: 'error' | 'warning';
} & React.HTMLAttributes<HTMLDivElement>;

export const Notification = ({
  message,
  className,
  state,
  ...props
}: NotificationProps) => {
  const { t } = useTranslation();

  const isError = state === 'error';
  const isWarning = state === 'warning';

  return (
    <div
      className={twMerge(
        'mb-6 rounded border-t-4 px-4 py-3',
        isError && 'border-red-500 bg-red-100 text-red-900',
        isWarning && 'border-yellow-500 bg-yellow-100 text-yellow-900',
        className,
      )}
      role="alert"
      {...props}
    >
      <div className="flex">
        <div className="py-1">
          <ExclamationCircleIcon
            className={twMerge(
              'mr-4 h-6 w-6',
              isError && 'text-red-500',
              isWarning && 'text-yellow-500',
            )}
          />
        </div>
        <div>
          <p className="font-bold">{t(`notification.${state}.headline`)}</p>
          <p>{message}</p>
        </div>
      </div>
    </div>
  );
};
