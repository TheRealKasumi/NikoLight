import { currentTheme } from '../libs';
import logoDarkSrc from '/logo-dark.png';
import logoLightSrc from '/logo-light.png';

type LogoProps = React.ImgHTMLAttributes<HTMLImageElement>;

export const Logo = (props: LogoProps): JSX.Element => (
  <img
    src={currentTheme() === 'light' ? logoLightSrc : logoDarkSrc}
    {...props}
  />
);
