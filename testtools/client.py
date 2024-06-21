import websockets
import asyncio

import time

async def connectWS(index):
    while True:
        try:
            async with websockets.connect('ws://localhost:9002') as websocket:
                await websocket.send(f'Register')
                while True:
                    message = await websocket.recv()    
                    print(f"{index} >> {message}")
        except Exception as ex:
            print(f'{index} >> error',ex)
        await asyncio.sleep(1)



async def main():
    tasks = []
    for i in range(10):
        tasks += [asyncio.create_task(connectWS(i))]
        
    await asyncio.gather(*tasks)
    print("主任务完成")

asyncio.run(main())