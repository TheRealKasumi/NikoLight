import * as AlertDialogPrimitive from '@radix-ui/react-alert-dialog';
import cx from 'classnames';
import { useState } from 'react';
import { useTranslation } from 'react-i18next';

type AlertDialogProps = Omit<
  AlertDialogPrimitive.AlertDialogProps,
  'open' | 'onOpenChange'
> & {
  title: React.ReactNode;
  description: React.ReactNode;
  onConfirm: () => void;
};

export const AlertDialog = ({
  children,
  title,
  description,
  onConfirm,
  ...props
}: AlertDialogProps): JSX.Element => {
  const { t } = useTranslation();
  const [isOpen, setIsOpen] = useState(false);

  return (
    <AlertDialogPrimitive.Root
      open={isOpen}
      onOpenChange={setIsOpen}
      {...props}
    >
      <AlertDialogPrimitive.Trigger asChild>
        {children}
      </AlertDialogPrimitive.Trigger>

      <AlertDialogPrimitive.Portal>
        <AlertDialogPrimitive.Overlay className="fixed inset-0 z-20 bg-black/50" />
        <AlertDialogPrimitive.Content
          className={cx(
            'fixed z-50',
            'w-[95vw] max-w-md rounded-lg p-4 md:w-full',
            'top-[50%] left-[50%] -translate-x-[50%] -translate-y-[50%]',
            'bg-cloud dark:bg-gray',
            'text-slate dark:text-white',
            'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
          )}
        >
          <AlertDialogPrimitive.Title className="text-sm font-medium">
            {title}
          </AlertDialogPrimitive.Title>
          <AlertDialogPrimitive.Description className="mt-2 text-sm font-normal">
            {description}
          </AlertDialogPrimitive.Description>
          <div className="mt-4 flex justify-end space-x-2">
            <AlertDialogPrimitive.Cancel
              className={cx(
                'inline-flex select-none justify-center rounded-md px-4 py-2 text-sm font-medium',
                'border border-gray bg-transparent dark:border-white/10',
                'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
              )}
            >
              {t('alertDialog.close')}
            </AlertDialogPrimitive.Cancel>
            <AlertDialogPrimitive.Action
              onClick={onConfirm}
              className={cx(
                'inline-flex select-none justify-center rounded-md px-4 py-2 text-sm font-medium',
                'bg-sky text-white',
                'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
              )}
            >
              {t('alertDialog.confirm')}
            </AlertDialogPrimitive.Action>
          </div>
        </AlertDialogPrimitive.Content>
      </AlertDialogPrimitive.Portal>
    </AlertDialogPrimitive.Root>
  );
};
