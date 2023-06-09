import cx from 'classnames';
import { Logo } from '../components';

type LoadingProps = { overlay?: boolean };

export const Loading = ({ overlay = false }: LoadingProps): JSX.Element => {
  if (overlay) {
    return (
      <div>
        <div className="fixed inset-0 z-20 bg-gray/90" />
        <div
          className={cx(
            'fixed z-50',
            'w-full',
            'top-[50%] left-[50%] -translate-x-[50%] -translate-y-[50%]',
          )}
        >
          <Logo className="h-48" />
        </div>
      </div>
    );
  }

  return (
    <div className="-m-12 flex h-screen items-center justify-center">
      <Logo className="h-48" />
    </div>
  );
};
