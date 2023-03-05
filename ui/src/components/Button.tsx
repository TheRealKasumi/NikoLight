import { twMerge } from 'tailwind-merge';

type ButtonProps = {
  variant?: 'primary' | 'secondary';
  size?: 'small';
} & React.ButtonHTMLAttributes<HTMLButtonElement>;

export const Button = ({
  children,
  className,
  variant,
  size,
  ...props
}: ButtonProps) => (
  <button
    {...props}
    className={twMerge(
      'w-full rounded-md',
      size === 'small' ? ' py-1' : 'py-2',
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
);
