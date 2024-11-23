import websockets
import asyncio
import json
import time
registerCmd = { "cmd":"register", "key" : "frametotcp"}
async def connectWS(index):
    loop=0
    while True:
        try:
            async with websockets.connect('ws://localhost:9002') as websocket:
                #registerCmd to string

                await websocket.send(json.dumps(registerCmd,indent=4))
                while True:
                    message = await websocket.recv()    
                    print(f"{index} >> {message}")
        except Exception as ex:
            print(f'{loop}....{index} >> error',ex)
        await asyncio.sleep(1)
        loop+=1



async def main():
    tasks = []
    for i in range(10):
        tasks += [asyncio.create_task(connectWS(i))]
        
    await asyncio.gather(*tasks)
    print("主任务完成")

asyncio.run(main())