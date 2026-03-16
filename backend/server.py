# backend/server.py
# Opsiyonel Python wrapper (+10 puan)
import ctypes
import json
from http.server import BaseHTTPRequestHandler, HTTPServer
import os

# Shared library yolu
LIB_PATH = os.path.join(os.path.dirname(__file__), '..', 'ministream.so')

try:
    lib = ctypes.CDLL(LIB_PATH)
    lib.deney_json.restype = ctypes.c_char_p
    LIB_LOADED = True
except OSError as e:
    print(f"[UYARI] ministream.so yuklenemedi: {e}")
    print("[UYARI] Once 'make lib' komutuyla shared library olusturun.")
    LIB_LOADED = False

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/benchmark':
            if LIB_LOADED:
                sonuc_bytes = lib.deney_json()
                sonuc = sonuc_bytes.decode('utf-8') if sonuc_bytes else '{}'
            else:
                sonuc = json.dumps({
                    "hata": "ministream.so yuklenemedi",
                    "cozum": "make lib komutunu calistirin"
                })

            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(sonuc.encode('utf-8'))

        elif self.path in ('/', '/index.html', '/dashboard/index.html'):
            try:
                html_path = os.path.join(os.path.dirname(__file__), '..', 'dashboard', 'index.html')
                with open(html_path, 'r', encoding='utf-8') as f:
                    html = f.read()
                
                self.send_response(200)
                self.send_header('Content-Type', 'text/html; charset=utf-8')
                self.end_headers()
                self.wfile.write(html.encode('utf-8'))
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f"Hata: dashboard/index.html okunamadi: {e}".encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

    def log_message(self, format, *args):
        pass  # terminal ciktisini sustur

if __name__ == '__main__':
    port = 8765
    print(f"[MiniStream] Sunucu baslatiliyor: http://localhost:{port}")
    print(f"[MiniStream] Benchmark endpoint: http://localhost:{port}/benchmark")
    HTTPServer(('localhost', port), Handler).serve_forever()
