import React from 'react';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type InputTextProps<T extends FieldValues> = Omit<
  React.InputHTMLAttributes<HTMLInputElement>,
  'onBlur' | 'name' | 'onChange' | 'ref' | 'value' | 'type'
> &
  UseControllerProps<T>;

export const InputText = <T extends FieldValues>({
  className,
  ...props
}: InputTextProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <input
      type="text"
      className={twMerge(
        'w-full rounded-md p-1',
        'bg-cloud dark:bg-gray',
        'text-center text-sky placeholder-sky placeholder-opacity-75',
        'focus-visible:sky focus:outline-none focus-visible:ring focus-visible:ring-opacity-75',
        className,
      )}
      {...props}
      {...field}
    />
  );
};
