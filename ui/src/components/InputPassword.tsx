import { EyeIcon, EyeSlashIcon } from '@heroicons/react/24/outline';
import React, { useState } from 'react';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type InputPasswordProps<T extends FieldValues> = Omit<
  React.InputHTMLAttributes<HTMLInputElement>,
  'onBlur' | 'name' | 'onChange' | 'ref' | 'value' | 'type'
> &
  UseControllerProps<T>;

export const InputPassword = <T extends FieldValues>({
  className,
  ...props
}: InputPasswordProps<T>): JSX.Element => {
  const [isVisible, setVisible] = useState<boolean>(false);
  const { field } = useController(props);

  return (
    <div className="relative">
      <input
        type={isVisible ? 'text' : 'password'}
        className={twMerge(
          'w-full rounded-md p-1',
          'bg-neutral dark:bg-gray',
          'text-center text-sky placeholder-sky placeholder-opacity-75',
          'focus-visible:sky focus:outline-none focus-visible:ring focus-visible:ring-opacity-75',
          className,
        )}
        {...props}
        {...field}
      />
      {isVisible ? (
        <EyeSlashIcon
          className="absolute inset-y-0 right-0 h-8 w-8 cursor-pointer pr-3 text-zinc"
          onClick={() => setVisible(false)}
        />
      ) : (
        <EyeIcon
          className="absolute inset-y-0 right-0 h-8 w-8 cursor-pointer pr-3 text-zinc"
          onClick={() => setVisible(true)}
        />
      )}
    </div>
  );
};
