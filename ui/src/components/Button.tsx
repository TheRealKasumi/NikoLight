import { twMerge } from 'tailwind-merge';

type ButtonProps = React.ButtonHTMLAttributes<HTMLButtonElement>;

export const Button = ({ children, className, ...props }: ButtonProps) => (
  <button
    {...props}
    className={twMerge(
      'w-full rounded-md py-2 text-neutral',
      'disabled:opacity-50',
      'focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75',
      props.type === 'reset' ? 'border border-gray bg-slate' : 'bg-sky',
      className,
    )}
  >
    {children}
  </button>
);
