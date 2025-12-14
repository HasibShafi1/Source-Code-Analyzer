import http.server
import socketserver
import subprocess
import json
import os
import sys

PORT = 3000
BACKEND_EXE = os.path.join(os.getcwd(), 'backend', 'analyzer.exe')
WEB_DIR = os.path.join(os.getcwd(), 'web')

class Handler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Serve static files from the 'web' directory
        if self.path == '/':
            self.path = '/index.html'
        
        # Adjust path to map to web directory
        # SimpleHTTPRequestHandler serves from current directory by default
        # We want it to serve from ./web/
        f = self.send_head()
        if f:
            self.copyfile(f, self.wfile)
            f.close()

    def translate_path(self, path):
        # Override to root everything to WEB_DIR
        path = path.lstrip('/')
        return os.path.join(WEB_DIR, path)

    def do_POST(self):
        if self.path == '/api/analyze':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            
            try:
                data = json.loads(post_data)
                source_code = data.get('source', '')
                
                # Run C++ executable
                process = subprocess.Popen(
                    [BACKEND_EXE],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True
                )
                
                stdout, stderr = process.communicate(input=source_code)
                
                if process.returncode != 0 and not stdout:
                     self.send_response(500)
                     self.send_header('Content-Type', 'application/json')
                     self.end_headers()
                     self.wfile.write(json.dumps({'error': stderr}).encode())
                     return

                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(stdout.encode())
                
            except Exception as e:
                self.send_response(500)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'error': str(e)}).encode())
        else:
            self.send_error(404)

print(f"Serving at http://localhost:{PORT}")
with socketserver.TCPServer(("", PORT), Handler) as httpd:
    httpd.serve_forever()
