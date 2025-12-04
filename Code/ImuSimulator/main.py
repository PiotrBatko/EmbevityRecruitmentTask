import time
import zmq


def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    while True:
        # Wait for next request from client
        message = socket.recv()
        print(f"Received request: {message}")

        # Do some 'work'
        time.sleep(1)

        # Send reply back to client
        socket.send(b"World")


if __name__ == '__main__':
    main()
