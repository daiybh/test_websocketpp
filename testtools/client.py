import websockets
import asyncio
async def main():
    async with websockets.connect('ws://localhost:9002') as websocket:
        
        await websocket.send(f'Register')
        while True:            
            message = await websocket.recv()
            print(message)

import time
while True:
    try:
        asyncio.run(main())
    except Exception as ex:
        print('error',ex)
        pass
    time.sleep(1)