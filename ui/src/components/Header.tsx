import { ChevronLeftIcon } from '@heroicons/react/24/outline';
import { Link } from '@tanstack/react-router';
import React from 'react';
import { twMerge } from 'tailwind-merge';

type HeaderProps = {
  name: React.ReactNode;
  icon?: React.ReactElement;
} & React.HTMLAttributes<HTMLDivElement>;

export const Header = ({
  name,
  icon,
  className,
  ...props
}: HeaderProps): JSX.Element => (
  <div
    className={twMerge('flex flex-col justify-center', className)}
    {...props}
  >
    <div className="mb-12 flex flex-row">
      <div className="w-8 flex-none self-center">
        <Link to="/">
          <ChevronLeftIcon className="my-1 h-6 w-6" />
        </Link>
      </div>
      <div className="grow text-center">
        <h1 className="mr-8 text-3xl">{name}</h1>
      </div>
    </div>
    {icon && (
      <div className="mb-12">
        {React.cloneElement(icon, {
          className: twMerge(icon.props.className, 'mx-auto h-auto w-64'),
        })}
      </div>
    )}
  </div>
);
