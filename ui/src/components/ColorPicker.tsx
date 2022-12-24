import React from 'react';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type ColorPickerProps<T extends FieldValues> = Omit<
  React.InputHTMLAttributes<HTMLInputElement>,
  'onBlur' | 'name' | 'onChange' | 'ref' | 'value'
> &
  UseControllerProps<T>;

export const ColorPicker = <T extends FieldValues>({
  className,
  ...props
}: ColorPickerProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <input
      type="color"
      className={twMerge(
        'cursor-pointer bg-transparent',
        'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
        className,
      )}
      {...field}
      {...props}
    />
  );
};
