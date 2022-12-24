import React from 'react';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type InputTypeProps<T extends FieldValues> = Omit<
  React.InputHTMLAttributes<HTMLInputElement>,
  'onBlur' | 'name' | 'onChange' | 'ref' | 'value' | 'type'
> &
  UseControllerProps<T>;

export const InputType = <T extends FieldValues>({
  className,
  ...props
}: InputTypeProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <input
      type="file"
      className={twMerge(
        'block w-full rounded-md p-1',
        'bg-neutral dark:bg-gray',
        'text-slate placeholder-sky placeholder-opacity-75 dark:text-neutral',
        'file:mr-2 file:rounded-md file:border-0 file:bg-sky file:text-neutral',
        'focus-visible:sky focus:outline-none focus-visible:ring focus-visible:ring-opacity-75',
        className,
      )}
      {...props}
      name={field.name}
      onBlur={field.onBlur}
      onChange={(e) => {
        // See https://github.com/react-hook-form/react-hook-form/discussions/5394
        field.onChange(e.target.files);
      }}
      ref={field.ref}
    />
  );
};
