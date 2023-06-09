export const changeTheme = (theme: string) => {
  const elThemeColor = document.querySelector('meta[name=theme-color]');

  if (theme === 'light') {
    document.documentElement.classList.remove('dark');
    elThemeColor?.setAttribute('content', '#FFFFFF');
  } else {
    document.documentElement.classList.add('dark');
    elThemeColor?.setAttribute('content', '#161717');
  }
};

export const currentTheme = (): 'light' | 'dark' =>
  document.documentElement.classList.contains('dark') ? 'dark' : 'light';
