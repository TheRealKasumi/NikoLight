import { forwardRef } from 'react';
import { twMerge } from 'tailwind-merge';

export type ButtonProps = {
  variant?: 'primary' | 'secondary';
  size?: 'small';
} & React.ButtonHTMLAttributes<HTMLButtonElement>;

const Button = forwardRef<HTMLButtonElement, ButtonProps>(
  ({ children, className, variant, size, ...props }, forwardedRef) => (
    <button
      {...props}
      ref={forwardedRef}
      className={twMerge(
        'w-full rounded-md',
        size === 'small' ? ' px-2 py-1' : 'px-4 py-2',
        'disabled:opacity-50',
        'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
        props.type === 'reset' || variant === 'secondary'
          ? 'border border-gray bg-transparent'
          : 'bg-sky text-white',
        className,
      )}
    >
      {children}
    </button>
  ),
);

Button.displayName = 'Button';

export { Button };
