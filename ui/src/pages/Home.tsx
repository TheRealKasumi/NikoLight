import { ChevronRightIcon } from '@heroicons/react/24/outline';
import {
  AllRouteInfo,
  AnyRoute,
  Route,
  RouteInfo,
  useRouter,
} from '@tanstack/react-router';
import { useTranslation } from 'react-i18next';
import { version } from '../../package.json';
import { ReactComponent as Logo } from '../assets/logo.svg';
import { Animation } from '../components';
import i18n from '../i18n';
import { toPercentage } from '../libs';
import { useLed, useUi } from './api';

const flatRoutes = (route: Route<AllRouteInfo, RouteInfo>): AnyRoute[] =>
  route.childRoutes
    ?.flatMap((route) => {
      if (route.childRoutes) {
        return [...(flatRoutes(route) ?? [])];
      }
      if (route.options.meta?.isVisible ?? true) {
        return route;
      }
    })
    .filter((route): route is AnyRoute => !!route) ?? [];

export const Home = (): JSX.Element => {
  const { t } = useTranslation();
  const { data: ledData } = useLed();
  const { routeTree } = useRouter();
  const routes = flatRoutes(routeTree);

  useUi({
    onSuccess: (data) => {
      i18n.changeLanguage(data.language);
    },
  });

  return (
    <>
      <h1 className="text-3xl">TesLight</h1>
      <h2 className="text-base text-zinc">v{version}</h2>
      <Logo className="my-12 h-44" />
      {routes.map((route) => {
        const meta = route.options.meta;

        let brightness = undefined;
        let animation = undefined;
        if (meta && Number.isInteger(meta.zoneId)) {
          // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
          const zoneId = meta.zoneId!;
          brightness = toPercentage(ledData?.[zoneId].brightness ?? 0);
          animation = t(
            `zone.animationTypes.${Animation[ledData?.[zoneId].type ?? 0]}`,
          );
        }

        const isCustomAnimationActive =
          ledData?.[0].type === Animation.Custom && !!brightness && !!animation;

        return (
          <route.Link
            key={route.routeId}
            className="aria-disabled:cursor-not-allowed aria-disabled:text-zinc"
            disabled={isCustomAnimationActive}
          >
            <div className="my-6 flex flex-row">
              <div className="w-12 flex-none self-center">{meta?.icon}</div>
              <div className="grow">
                <p>{meta?.name}</p>
                {!!animation && (
                  <p className="text-xs">
                    {isCustomAnimationActive
                      ? t(`zone.animationTypes.${Animation[Animation.Custom]}`)
                      : animation}

                    {!!brightness && !isCustomAnimationActive && (
                      <span className="text-zinc">
                        {' '}
                        &bull; {t('zone.brightness')} {brightness}%
                      </span>
                    )}
                  </p>
                )}
              </div>
              <div className="w-6 flex-none self-center text-right text-zinc">
                <ChevronRightIcon className="h-4 w-4" />
              </div>
            </div>
          </route.Link>
        );
      })}
    </>
  );
};
