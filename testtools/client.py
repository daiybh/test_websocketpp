import websockets
import asyncio
async def main():
    async with websockets.connect('ws://localhost:9002') as websocket:
        while True:
            message = await websocket.recv()
            print(message)


asyncio.get_event_loop().run_until_complete(main())