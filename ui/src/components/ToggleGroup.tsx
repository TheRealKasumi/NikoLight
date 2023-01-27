import * as ToggleGroupPrimitive from '@radix-ui/react-toggle-group';
import classNames from 'classnames';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';

type ToggleGroupProps<T extends FieldValues> = Omit<
  | ToggleGroupPrimitive.ToggleGroupSingleProps
  | ToggleGroupPrimitive.ToggleGroupMultipleProps,
  'name' | 'onBlur' | 'onValueChange' | 'ref' | 'value'
> &
  UseControllerProps<T>;

export const ToggleGroup = <T extends FieldValues>({
  children,
  ...props
}: ToggleGroupProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <ToggleGroupPrimitive.Root
      {...props}
      className={classNames(
        'grid grid-cols-4 gap-3 rounded-md p-3 md:grid-cols-8',
        'bg-cloud dark:bg-gray',
      )}
      onBlur={field.onBlur}
      onValueChange={field.onChange}
      ref={field.ref}
      value={Array.isArray(field.value) ? field.value : undefined}
    >
      {children}
    </ToggleGroupPrimitive.Root>
  );
};

export const ToggleGroupItem = ({
  children,
  ...props
}: ToggleGroupPrimitive.ToggleGroupItemProps): JSX.Element => (
  <ToggleGroupPrimitive.Item
    {...props}
    key={`group-item-${props.value}-${props.title}`}
    aria-label={props.title}
    className={classNames(
      'rounded-md py-2',
      'bg-white radix-state-on:bg-sky radix-state-on:text-white dark:bg-zinc dark:text-white radix-state-on:dark:bg-sky',
      'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
    )}
  >
    {children}
  </ToggleGroupPrimitive.Item>
);
