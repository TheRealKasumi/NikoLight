import * as SwitchPrimitive from '@radix-ui/react-switch';
import classNames from 'classnames';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type SwitchProps<T extends FieldValues> = Omit<
  SwitchPrimitive.SwitchProps,
  'name' | 'onBlur' | 'onCheckedChange' | 'ref' | 'checked'
> &
  UseControllerProps<T>;

export const Switch = <T extends FieldValues>({
  className,
  ...props
}: SwitchProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <SwitchPrimitive.Root
      className={twMerge(
        'group',
        'radix-state-checked:bg-sky',
        'radix-state-unchecked:bg-zinc dark:radix-state-unchecked:bg-gray',
        'relative inline-flex h-[24px] w-[44px] flex-shrink-0 cursor-pointer rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out',
        'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
        className,
      )}
      {...props}
      checked={field.value}
      name={field.name}
      onBlur={field.onBlur}
      onCheckedChange={field.onChange}
      ref={field.ref}
    >
      <SwitchPrimitive.Thumb
        className={classNames(
          'group-radix-state-checked:translate-x-5',
          'group-radix-state-unchecked:translate-x-0',
          'pointer-events-none inline-block h-[20px] w-[20px] transform rounded-full bg-cloud ring-0 transition duration-200 ease-in-out dark:bg-neutral',
        )}
      />
    </SwitchPrimitive.Root>
  );
};
