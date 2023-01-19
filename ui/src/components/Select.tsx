import {
  CheckIcon,
  ChevronDownIcon,
  ChevronUpIcon,
} from '@heroicons/react/24/outline';
import * as SelectPrimitive from '@radix-ui/react-select';
import classNames from 'classnames';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';

type SelectProps<T extends FieldValues> = Omit<
  SelectPrimitive.SelectProps,
  'name' | 'ref' | 'value'
> &
  UseControllerProps<T>;

export const Select = <T extends FieldValues>({
  children,
  onValueChange,
  ...props
}: SelectProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <SelectPrimitive.Root
      {...props}
      name={field.name}
      onValueChange={(value) => {
        field.onChange(value);
        onValueChange && onValueChange(value);
      }}
      value={field.value}
    >
      <SelectPrimitive.Trigger
        ref={field.ref}
        className={classNames(
          'inline-flex select-none items-center justify-center rounded-md px-4 py-2 text-sm font-medium',
          'bg-neutral text-sky dark:bg-gray',
          'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
          'group',
        )}
      >
        <SelectPrimitive.Value />
        <SelectPrimitive.Icon className="ml-2">
          <ChevronDownIcon className="h-4 w-4" />
        </SelectPrimitive.Icon>
      </SelectPrimitive.Trigger>
      <SelectPrimitive.Portal className="text-sky">
        <SelectPrimitive.Content>
          <SelectPrimitive.ScrollUpButton className="flex items-center justify-center">
            <ChevronUpIcon className="h-4 w-4" />
          </SelectPrimitive.ScrollUpButton>
          <SelectPrimitive.Viewport className="rounded-lg bg-neutral p-2 shadow-lg dark:bg-gray">
            {children}
          </SelectPrimitive.Viewport>
          <SelectPrimitive.ScrollDownButton className="flex items-center justify-center">
            <ChevronDownIcon className="h-4 w-4" />
          </SelectPrimitive.ScrollDownButton>
        </SelectPrimitive.Content>
      </SelectPrimitive.Portal>
    </SelectPrimitive.Root>
  );
};

export const SelectItem = ({
  children,
  ...props
}: SelectPrimitive.SelectItemProps): JSX.Element => (
  <SelectPrimitive.Item
    {...props}
    className={classNames(
      'relative flex items-center px-8 py-2 text-sm font-medium text-sky',
      'radix-disabled:opacity-50',
      'select-none focus:outline-none',
    )}
  >
    <SelectPrimitive.ItemText>{children}</SelectPrimitive.ItemText>
    <SelectPrimitive.ItemIndicator className="absolute left-2 inline-flex items-center">
      <CheckIcon className="h-4 w-4" />
    </SelectPrimitive.ItemIndicator>
  </SelectPrimitive.Item>
);
