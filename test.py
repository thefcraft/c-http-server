import requests
import socket

HOST, PORT = ('127.0.0.1', 8080)
url = f'http://{HOST}:{PORT}/'

def test_raw():
    # Define HTTP request
    request = b"GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\nSend for testing!"

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.sendall(request)
    # Receive the response
    response = b""
    while True:
        data = s.recv(1024)
        if not data: break
        response += data

    # Print the raw response data
    print(response)

    # Close the socket
    s.close()

def test_get():
    # Send GET request to the server
    response = requests.get(url)
    # # Print raw response data
    print(response.headers)
    print(response.content)
def test_post():
    myobj = {'somekey': 'somevalue', 'somekey2': 'somevalue2', 'somekey3': [i for i in range(6)]}
    r = requests.post(url+'api', json = myobj)
    print(r.headers)
    print(r.content)

if __name__ == "__main__":
    test_get()
    # test_post()
    # test_raw()