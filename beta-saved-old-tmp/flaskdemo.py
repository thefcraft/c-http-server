from flask import Flask, Response, render_template, send_file, jsonify, request, redirect, url_for
import os
# from pdf2image import convert_from_bytes
# from PyPDF2 import PdfWriter, PdfReader
import io
import base64
import cv2

# flask variables
app = Flask(__name__)

# PATH = r"C:/ThefCraft/"
PATH = r'lab'
icon_mapping = {
    
    'ai': 'adobe-illustrator.png',
    'apk': 'apk.png',
    'css': 'css.png',
    'disc': 'disc.png',
    'doc': 'word.png',
    'docx': 'word.png',
    'eot': 'font-file.png',  # Change this to 'ttf.png' or 'otf.png' if desired
    'excel': 'excel.png',
    'flac': 'music.png',  # Assuming this is a music file extension
    'gif': 'image.png',  # Assuming this is an image file extension
    'html': 'text.png',  # Assuming this is a text file extension
    'image': 'image.png',
    'iso': 'iso.png',
    'java': 'js-file.png',  # Change this to 'javascript.png' if desired
    'javascript': 'js-file.png',  # Change this to 'javascript.png' if desired
    'jpeg': 'image.png',  # Assuming this is an image file extension
    'jpg': 'image.png',  # Assuming this is an image file extension
    'js': 'js-file.png',  # Change this to 'javascript.png' if desired
    'jsx': 'js-file.png',  # Change this to 'javascript.png' if desired
    'mail': 'mail.png',
    'mkv': 'video.png',  # Assuming this is a video file extension
    'mp3': 'music.png',
    'mp4': 'video.png',  # Assuming this is a video file extension
    'mpeg': 'video.png',  # Assuming this is a video file extension
    'mpg': 'video.png',  # Assuming this is a video file extension
    'music': 'music.png',
    'ogg': 'music.png',  # Assuming this is a music file extension
    'pdf': 'pdf.png',
    'php': 'php.png',
    'png': 'image.png',  # Assuming this is an image file extension
    'powerpoint': 'powerpoint.png',
    'ppt': 'powerpoint.png',
    'psd': 'psd.png',
    'record': 'record.png',
    'sql': 'sql.png',
    'svg': 'svg.png',
    'text': 'text.png',
    'ttf': 'ttf.png',  # Change this to 'font-file.png' or 'otf.png' if desired
    'txt': 'text.png',
    'vector': 'vector.png',
    'vob': 'video.png',  # Assuming this is a video file extension
    'wav': 'music.png',  # Assuming this is a music file extension
    'webm': 'video.png',  # Assuming this is a video file extension
    'webp': 'image.png',  # Assuming this is an image file extension
    'word': 'word.png',
    'xls': 'excel.png',
    'xlsx': 'excel.png',
    'xml': 'text.png',  # Assuming this is a text file extension
    'zip': 'zip.png',
}


def getIcon(path:str):
    ext = os.path.splitext(path)[1].removeprefix('.')
    return icon_mapping.get(ext, "javascript.png")
def getGridViewIcon(path:str):
    ext = os.path.splitext(path)[1].removeprefix('.')
    if ext in ['png', 'jpg', 'jpeg']:
        return f'files/{path}'
    elif ext in ['mp4', 'webm', 'vob', 'mpg', 'mpeg', 'mkv']:
        return f'api/getImageFromVideo/{path}' # return f'data:image/png;base64, {base64.b64encode(thumb_buf.tobytes()).decode()}'
        
CHUNK_SIZE = 128
# TEMP_STORAGE = {}
def preProcesser(path, chunk_idx=0):
    result = []
    client_path = path
    path = os.path.join(PATH, path)
    if not os.path.exists(path): return result
    # files = TEMP_STORAGE.get(path, None)
    # if files:
    #     print('loading files from %s via chunk so if any changes not visable' % path)
    # else:
    #     files = os.listdir(path)
    #     TEMP_STORAGE.update({
    #         path: files
    #     })
    files = os.listdir(path)
    
    for file in files[chunk_idx*CHUNK_SIZE:chunk_idx*CHUNK_SIZE+CHUNK_SIZE]:
        fullPath = os.path.join(path, file)
        result.append((os.path.join(client_path, file), os.path.isdir(fullPath), file, getIcon(fullPath), getGridViewIcon(fullPath)))
    return sorted(result, key=lambda x: x[1], reverse=True)

def getNavigation(path):
    result = [(path, os.path.split(path)[1])]
    while True:
        path = os.path.split(path)[0]
        if path == '': break
        result.append((path, os.path.split(path)[1]))
    return [('', 'root')]+result[::-1]
@app.route('/api/upload', methods=['POST'])
def upload():
    for fpathuser, fobj in request.files.items():
        fpath = fpathuser.split('|files_')[0]
        
        fobj.save(os.path.join(os.path.join(PATH, fpath), fobj.filename))
        print('UPLOADING', fobj.filename, 'TO', os.path.join(os.path.join(PATH, fpath), fobj.filename))
    return 'OK'
@app.route('/api/getImageFromVideo/<path:path>')
def getThumbnailFromVideo(path):
    threshold = 10
    vcap = cv2.VideoCapture(path)
    res, im_ar = vcap.read()
    # while im_ar.mean() < threshold and res: res, im_ar = vcap.read()
    res, thumb_buf = cv2.imencode('.png', im_ar)
    bte = thumb_buf.tobytes()
    vcap.release()
    return bte

@app.route('/api/load', methods=['POST'])
def load(): 
    idx = request.json.get("idx")
    path = request.json.get("path")
    print(path, idx)
    return jsonify({
        'files':preProcesser(path, chunk_idx=idx)
        })
@app.route('/')
def reload():
    return '''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; URL='/files/'">
    <title>Redirecting...</title>
</head>
<body>
    <p>Redirecting to /files...</p>
</body>
</html>'''
    # return redirect('/files/')

@app.route('/files/', defaults={'path': ''})
@app.route('/files/<path:path>')
def get_dir(path):
    
    if not os.path.exists(path) and path == 'favicon.ico': return send_file('./static/icon.svg')
    if not os.path.exists(path) and path == 'favicon.ico': return send_file('./static/icon.svg')
    if not os.path.realpath(os.path.join(PATH, path)).startswith(os.path.realpath(PATH)):
        return '''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; URL='/files/'">
    <title>Redirecting...</title>
</head>
<body>
    <p>Redirecting to /files...</p>
</body>
</html>'''
        
    if os.path.isdir(os.path.join(PATH, path)):
        return render_template('index.html', files=preProcesser(path), navigation=getNavigation(path))
    else:
        return send_file(os.path.join(PATH, path))
    
if __name__ == '__main__': 
    # app.run()
    app.run(debug=False, host='0.0.0.0', port=80)#, ssl_context=('cert.pem', 'key.pem'))