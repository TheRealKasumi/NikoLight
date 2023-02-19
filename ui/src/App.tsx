import {
  AdjustmentsVerticalIcon,
  ArrowPathIcon,
  DocumentTextIcon,
  FilmIcon,
} from '@heroicons/react/24/outline';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { ReactQueryDevtools } from '@tanstack/react-query-devtools';
import {
  createMemoryHistory,
  Outlet,
  ReactRouter,
  RootRoute,
  Route,
  RouterProvider,
} from '@tanstack/react-router';
import { lazy } from 'react';
import { Trans } from 'react-i18next';
import { ReactComponent as CenterConsoleIcon } from './assets/center-console.svg';
import { ReactComponent as DashboardIcon } from './assets/dashboard.svg';
import { ReactComponent as DoorIcon } from './assets/door.svg';
import { ReactComponent as FootwellLeftIcon } from './assets/footwell-left.svg';
import { ReactComponent as FootwellRightIcon } from './assets/footwell-right.svg';
import { Error, Header, Loading } from './components';
import {
  CustomAnimations,
  Home,
  LogFiles,
  Settings,
  Update,
  Zone,
} from './pages';

const basePath = import.meta.env.BASE_URL;

const TanStackRouterDevtools = import.meta.env.PROD
  ? () => null
  : lazy(() =>
      // Lazy load in development
      import('@tanstack/react-router-devtools').then((res) => ({
        default: res.TanStackRouterDevtools,
      })),
    );

const rootRoute = new RootRoute({
  component: () => (
    <>
      <Outlet />
      <TanStackRouterDevtools position="bottom-right" />
    </>
  ),
});

const homeRoute = new Route({
  getParentRoute: () => rootRoute,
  path: '/',
  component: Home,
  meta: {
    name: <Trans i18nKey="home.title" />,
    isVisible: false,
  },
});

const zonesRoute = new Route({
  getParentRoute: () => rootRoute,
  path: 'zones',
  meta: {
    name: <Trans i18nKey="zones.title" />,
    isVisible: false,
  },
  component: () => {
    const currentRoute = router
      .matchRoutes(router.state.currentLocation.pathname)
      .reverse()[0].route;

    const meta = currentRoute.options.meta;

    return (
      <>
        <Header
          name={meta?.name ?? currentRoute.id}
          {...(meta?.icon && { icon: meta.icon })}
        />
        <Outlet />
      </>
    );
  },
});

const dashboardRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'dashboard',
  component: () => <Zone routeId={dashboardRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.dashboard" />,
    icon: <DashboardIcon className="h-8 w-8 stroke-slate dark:stroke-white" />,
    zoneId: 0,
  },
});

const centerConsoleRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'center-console',
  component: () => <Zone routeId={centerConsoleRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.centerConsole" />,
    icon: (
      <CenterConsoleIcon className="h-8 w-8 stroke-slate dark:stroke-white" />
    ),
    zoneId: 1,
  },
});

const frontLeftDoorRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'front-left-door',
  component: () => <Zone routeId={frontLeftDoorRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.frontLeftDoor" />,
    icon: (
      <DoorIcon className="h-8 w-8 -scale-x-100 stroke-slate dark:stroke-white" />
    ),
    zoneId: 2,
  },
});

const frontRightDoorRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'front-right-door',
  component: () => <Zone routeId={frontRightDoorRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.frontRightDoor" />,
    icon: <DoorIcon className="h-8 w-8 stroke-slate dark:stroke-white" />,
    zoneId: 3,
  },
});

const rearLeftDoorRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'rear-left-door',
  component: () => <Zone routeId={rearLeftDoorRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.rearLeftDoor" />,
    icon: (
      <DoorIcon className="h-8 w-8 -scale-x-100 stroke-slate dark:stroke-white" />
    ),
    zoneId: 4,
  },
});

const rearRightDoorRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'rear-right-door',
  component: () => <Zone routeId={rearRightDoorRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.rearRightDoor" />,
    icon: <DoorIcon className="h-8 w-8 stroke-slate dark:stroke-white" />,
    zoneId: 5,
  },
});

const leftFootwellRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'left-footwell',
  component: () => <Zone routeId={leftFootwellRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.leftFootwell" />,
    icon: (
      <FootwellLeftIcon className="h-8 w-8 stroke-slate dark:stroke-white" />
    ),
    zoneId: 6,
  },
});

const rightFootwellRoute = new Route({
  getParentRoute: () => zonesRoute,
  path: 'right-footwell',
  component: () => <Zone routeId={rightFootwellRoute.id} />,
  meta: {
    name: <Trans i18nKey="zones.rightFootwell" />,
    icon: (
      <FootwellRightIcon className="h-8 w-8 stroke-slate dark:stroke-white" />
    ),
    zoneId: 7,
  },
});

const customAnimationsRoute = new Route({
  getParentRoute: () => rootRoute,
  path: 'custom-animations',
  component: () => {
    const meta = customAnimationsRoute.options.meta;
    return (
      <>
        <Header name={meta?.name ?? customAnimationsRoute.id} />
        <CustomAnimations />
      </>
    );
  },
  meta: {
    name: <Trans i18nKey="customAnimations.title" />,
    icon: <FilmIcon className="h-6 w-6 stroke-slate dark:stroke-white" />,
  },
});

const settingsRoute = new Route({
  getParentRoute: () => rootRoute,
  path: 'settings',
  component: () => {
    const meta = settingsRoute.options.meta;
    return (
      <>
        <Header name={meta?.name ?? settingsRoute.id} />
        <Settings />
      </>
    );
  },
  meta: {
    name: <Trans i18nKey="settings.title" />,
    icon: (
      <AdjustmentsVerticalIcon className="h-6 w-6 stroke-slate dark:stroke-white" />
    ),
  },
});

const updateRoute = new Route({
  getParentRoute: () => rootRoute,
  path: 'update',
  component: () => {
    const meta = updateRoute.options.meta;
    return (
      <>
        <Header name={meta?.name ?? updateRoute.id} />
        <Update />
      </>
    );
  },
  meta: {
    name: <Trans i18nKey="update.title" />,
    icon: <ArrowPathIcon className="h-6 w-6" />,
  },
});

const logFilesRoute = new Route({
  getParentRoute: () => rootRoute,
  path: 'log-files',
  component: () => {
    const meta = logFilesRoute.options.meta;
    return (
      <>
        <Header name={meta?.name ?? logFilesRoute.id} />
        <LogFiles />
      </>
    );
  },
  meta: {
    name: <Trans i18nKey="logFiles.title" />,
    icon: (
      <DocumentTextIcon className="h-6 w-6 stroke-slate dark:stroke-white" />
    ),
  },
});

const routeTree = rootRoute.addChildren([
  homeRoute,
  zonesRoute.addChildren([
    dashboardRoute,
    centerConsoleRoute,
    frontLeftDoorRoute,
    frontRightDoorRoute,
    rearLeftDoorRoute,
    rearRightDoorRoute,
    leftFootwellRoute,
    rightFootwellRoute,
  ]),
  customAnimationsRoute,
  settingsRoute,
  updateRoute,
  logFilesRoute,
]);

const memoryHistory = createMemoryHistory({
  initialEntries: [basePath.slice(0, -1)],
});

const router = new ReactRouter({
  routeTree,
  basepath: basePath,
  history: memoryHistory,
});

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      retry: 0,
      suspense: true,
    },
  },
});

const App = () => (
  <div className=" bg-white text-slate dark:bg-slate dark:text-white">
    <div className="container mx-auto min-h-screen p-12">
      <QueryClientProvider client={queryClient}>
        <RouterProvider
          router={router}
          defaultErrorComponent={Error}
          defaultPendingComponent={Loading}
        />
        <ReactQueryDevtools />
      </QueryClientProvider>
    </div>
  </div>
);

export default App;
