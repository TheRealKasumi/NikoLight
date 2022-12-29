import { ExclamationCircleIcon } from '@heroicons/react/24/outline';
import { useTranslation } from 'react-i18next';
import { twMerge } from 'tailwind-merge';

type NotificationProps = {
  message: string;
} & React.HTMLAttributes<HTMLDivElement>;

export const Notification = ({
  message,
  className,
  ...props
}: NotificationProps) => {
  const { t } = useTranslation();

  return (
    <div
      className={twMerge(
        'mb-6 rounded border-t-4 border-red-500 bg-red-100 px-4 py-3 text-red-900',
        className,
      )}
      role="alert"
      {...props}
    >
      <div className="flex">
        <div className="py-1">
          <ExclamationCircleIcon className="mr-4 h-6 w-6 text-red-500" />
        </div>
        <div>
          <p className="font-bold">{t('notification.headline')}</p>
          <p>{message}</p>
        </div>
      </div>
    </div>
  );
};
