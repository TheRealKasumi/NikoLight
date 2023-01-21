import React from 'react';
import { twMerge } from 'tailwind-merge';

type LogProps = React.HTMLAttributes<HTMLPreElement>;

export const Log = ({
  className,
  children,
  ...props
}: LogProps): JSX.Element => (
  <pre
    className={twMerge(
      'w-full rounded-md p-3 text-xs',
      'bg-cloud dark:bg-gray',
      className,
    )}
    {...props}
  >
    {children}
  </pre>
);
