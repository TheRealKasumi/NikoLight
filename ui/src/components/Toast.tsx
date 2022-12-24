import * as ToastPrimitive from '@radix-ui/react-toast';
import cx from 'classnames';

type ToastProps = {
  title?: string;
  content?: string;
} & ToastPrimitive.ToastProps;

export const Toast = ({
  title,
  content,
  ...props
}: ToastProps): JSX.Element => (
  <ToastPrimitive.Provider>
    <ToastPrimitive.Root
      {...props}
      className={cx(
        'fixed inset-x-4 bottom-4 z-50 w-auto rounded-lg shadow-lg md:top-4 md:right-4 md:left-auto md:bottom-auto md:w-full md:max-w-sm',
        'bg-neutral dark:bg-gray',
        'radix-state-open:animate-toast-slide-in-bottom md:radix-state-open:animate-toast-slide-in-right',
        'radix-state-closed:animate-toast-hide',
        'radix-swipe-end:animate-toast-swipe-out',
        'translate-x-radix-toast-swipe-move-x',
        'radix-swipe-cancel:translate-x-0 radix-swipe-cancel:duration-200 radix-swipe-cancel:ease-[ease]',
        'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
      )}
    >
      <div className="flex">
        <div className="flex w-0 flex-1 items-center py-4 pl-5">
          <div className="radix w-full">
            {title && (
              <ToastPrimitive.Title className="text-sm font-medium">
                {title}
              </ToastPrimitive.Title>
            )}
            {content && (
              <ToastPrimitive.Description className="mt-1 text-sm">
                {content}
              </ToastPrimitive.Description>
            )}
          </div>
        </div>
        <div className="flex space-y-1 border-l border-gray border-opacity-5 px-3 py-2 dark:border-neutral dark:border-opacity-5">
          <ToastPrimitive.Close className="flex w-full items-center justify-center rounded-lg px-3 py-2 text-sm font-medium focus:z-10 focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75">
            Dismiss
          </ToastPrimitive.Close>
        </div>
      </div>
    </ToastPrimitive.Root>

    <ToastPrimitive.Viewport />
  </ToastPrimitive.Provider>
);
