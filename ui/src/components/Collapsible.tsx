import {
  ArrowsPointingInIcon,
  ArrowsPointingOutIcon,
} from '@heroicons/react/24/outline';
import * as CollapsiblePrimitive from '@radix-ui/react-collapsible';
import React from 'react';
import { twMerge } from 'tailwind-merge';

type CollapsibleProps = Omit<
  CollapsiblePrimitive.CollapsibleProps,
  'open' | 'onOpenChange'
>;

export const Collapsible = ({
  className,
  defaultOpen = false,
  ...props
}: CollapsibleProps): JSX.Element => {
  const [isOpen, setIsOpen] = React.useState<boolean>(defaultOpen);

  return (
    <CollapsiblePrimitive.Root
      open={isOpen}
      onOpenChange={setIsOpen}
      {...props}
    >
      <div
        className={twMerge(
          'group flex w-full select-none items-center justify-between',
          'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
          className,
        )}
      >
        <span className="font-medium underline">{props.title}</span>
        <CollapsiblePrimitive.Trigger asChild>
          <button>
            {isOpen ? (
              <ArrowsPointingInIcon className="h-6 w-6" />
            ) : (
              <ArrowsPointingOutIcon className="h-6 w-6" />
            )}
          </button>
        </CollapsiblePrimitive.Trigger>
      </div>

      <CollapsiblePrimitive.Content className="mt-4 px-6">
        {props.children}
      </CollapsiblePrimitive.Content>
    </CollapsiblePrimitive.Root>
  );
};
