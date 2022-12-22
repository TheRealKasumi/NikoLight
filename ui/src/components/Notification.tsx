import { ExclamationCircleIcon } from '@heroicons/react/24/outline';
import { twMerge } from 'tailwind-merge';

type NotificationProps = {
  title?: string;
  message: string;
} & React.HTMLAttributes<HTMLDivElement>;

export const Notification = ({
  title = 'Something seriously bad happened',
  message,
  className,
  ...props
}: NotificationProps) => (
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
        <p className="font-bold">{title}</p>
        <p>{message}</p>
      </div>
    </div>
  </div>
);
