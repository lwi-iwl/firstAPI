#include <windows.h>

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  
  bool isDown = false;
  bool isOut = false;
  int direction = 0;
  DWORD msec = 0;
  SYSTEMTIME st;

class Sprite{
  private:
    HBITMAP bmpSource = NULL;
    HDC oldDC;
    HDC newDC;
    HBITMAP oldBmp;
    BITMAP bm;
    PAINTSTRUCT ps;
    RECT rect;
    HDC hdcDestination;
    int oldx = 100;
    int oldy = 100;
    int relx = 0;
    int rely = 0;
    int oldsecondx;
    int oldsecondy;
  public:
    HWND hWnd;

    void preparation(){
      bmpSource = (HBITMAP)::LoadImage(NULL, "C:\\Docs\\OS\\1\\pictur.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
      GetObject(bmpSource, sizeof( bm ), &bm );
      oldsecondx = oldx + bm.bmWidth;
      oldsecondy = oldy + bm.bmHeight;
      oldDC = GetDC(hWnd);
      newDC = CreateCompatibleDC(oldDC);
      oldBmp = (HBITMAP)SelectObject(newDC, bmpSource);
      GetClientRect (hWnd, &rect);
    }



    void create(int x, int y){
      if ((((x-relx) > 0) && ((y-rely) > 0) && ((x+bm.bmWidth-relx) < 1280) && ((y+bm.bmHeight-rely)<700)))
      {
      FillRect (oldDC, &rect, (HBRUSH)(COLOR_WINDOW+1));
      BitBlt(oldDC, x-relx, y-rely, bm.bmWidth, bm.bmHeight, newDC, 0, 0, SRCCOPY);
      //SwapBuffers(newDC);
      this -> setOldParam(x, y);
      this -> setOldSecondParam();
      }
      else{
      if ((msec != 0)&&(GetTickCount()-msec > 5000)){
        this -> setRelativeParam(oldx, oldy);
        //MessageBox (hWnd,"nm,mn","hjhj", NULL);
        if ((x > oldx) && (y>oldy))
          if ((y+bm.bmHeight-rely)>=700)
            direction = 1;
          else
            direction = 3;
        else if ((x > oldx) && (y<oldy))
          if ((y-rely) <= 0)
            direction = 0;
          else
            direction = 2;
        else if ((x < oldx) && (y<oldy))
          if ((y-rely) <= 0)
            direction = 3;
          else
            direction = 1;
        else if ((x < oldx) && (y>oldy))
          if ((x-relx) <= 0)
            direction = 0;
          else
            direction = 2;

      }
      isOut = true;
      }
    }

    void setOutFalse (){
      isOut = false;
      if (GetAsyncKeyState(VK_LBUTTON) >= 0){
        isDown = false;
        this -> setRelativeParam(oldx, oldy);
      }
      else
        SetCursor(LoadCursor(NULL, IDC_HAND));
    }

    void setRelativeParam(int x, int y){
      relx = x - oldx;
      rely = y - oldy;
    }

    void setOldSecondParam(){
      oldsecondx = oldx + bm.bmWidth;
      oldsecondy = oldy + bm.bmHeight;
    }

    void Clean(){
      SelectObject(newDC, oldBmp); 
      DeleteDC(newDC);
      ReleaseDC(hWnd, oldDC);
    }

    void setOldParam(int x , int y){
      oldx = x - relx;
      oldy = y - rely;
    }


    int getX(){
      return oldx;
    }

    int getY(){
      return oldy;
    }

    int getSecondX(){
      return oldsecondx;
    }

    int getSecondY(){
      return oldsecondy;
    }


};
Sprite sprite;
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
  WNDCLASS windowClass = { 0 };
  windowClass.lpfnWndProc = WindowProc;
  windowClass.hInstance = hInstance;
  windowClass.lpszClassName = "HELLO_WORLD";
  RegisterClass(&windowClass);
  HWND hwnd = CreateWindow(
    windowClass.lpszClassName,
    "CatXP",
    WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
    100, 50, 1280, 720,
    nullptr, nullptr,
    hInstance,
    nullptr);
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);
  sprite.hWnd = hwnd;
  sprite.preparation();
  sprite.create(100, 100);
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  MSG msg = {};
  while (msg.message != WM_QUIT)
  {
    if (msec == 0)
      msec = GetTickCount();
    if (GetTickCount()-msec > 5000)
    {
      if (direction == 0)
        sprite.create(sprite.getX()+5, sprite.getY()+5);
      else if (direction == 1)
        sprite.create(sprite.getX()+5, sprite.getY()-5);
      else if (direction == 2)
        sprite.create(sprite.getX()-5, sprite.getY()-5);
      else if (direction == 3)
        sprite.create(sprite.getX()-5, sprite.getY()+5);
      Sleep(20);
    }

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  sprite.Clean();
  return 0;
}

__int64 __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WORD xPos, yPoz;
  POINT pt;
  switch (message)
  {


  case WM_LBUTTONDOWN:
    msec = 0;
    GetCursorPos (&pt);
    ScreenToClient (hWnd, &pt);
    if ((sprite.getX() <= pt.x) && (sprite.getY() <= pt.y) && (sprite.getSecondX() >= pt.x) && (sprite.getSecondY() >= pt.y)){
    SetCursor(LoadCursor(NULL, IDC_HAND));
    sprite.setRelativeParam(pt.x, pt.y);
    isDown = true;
    isOut = false;
    }
    break;    
  
  case WM_LBUTTONUP:
    msec = 0;
    if (isDown){
      GetCursorPos (&pt);
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      ScreenToClient (hWnd, &pt);
      sprite.setOldParam(pt.x, pt.y);
      sprite.setOldSecondParam();
      sprite.setRelativeParam(sprite.getX(), sprite.getY());
      isDown = false;
    }
    break;

  case WM_MOUSEMOVE:
    msec = 0;
    if (isOut)
      sprite.setOutFalse();
    if (isDown){
      GetCursorPos (&pt);
      ScreenToClient (hWnd, &pt);
      sprite.create(pt.x, pt.y);
    }
    break;

  case WM_MOUSEWHEEL:
    msec = 0;
    if (LOWORD(wParam) == MK_CONTROL)
      sprite.create(sprite.getX()+GET_WHEEL_DELTA_WPARAM(wParam), sprite.getY());   
    else
      sprite.create(sprite.getX(), sprite.getY()+GET_WHEEL_DELTA_WPARAM(wParam));
    break;

  case WM_KEYDOWN:
    msec = 0;
    if(wParam == VK_NUMPAD2)
      sprite.create(sprite.getX(), sprite.getY()+10);
    else if (wParam == VK_NUMPAD8)
      sprite.create(sprite.getX(), sprite.getY()-10);
    else if (wParam == VK_NUMPAD6)
      sprite.create(sprite.getX()+10, sprite.getY());
    else if (wParam == VK_NUMPAD4)
      sprite.create(sprite.getX()-10, sprite.getY());
    else if (wParam == VK_NUMPAD7)
      sprite.create(sprite.getX()-10, sprite.getY()-10);
    else if (wParam == VK_NUMPAD9)
      sprite.create(sprite.getX()+10, sprite.getY()-10);
    else if (wParam == VK_NUMPAD1)
      sprite.create(sprite.getX()-10, sprite.getY()+10);
    else if (wParam == VK_NUMPAD3)
      sprite.create(sprite.getX()+10, sprite.getY()+10);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);

  }

  return 0;
}
