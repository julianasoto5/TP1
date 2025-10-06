Para correr el cliente 
```
cd client 
npm install
npm run dev
```

Para correr el servidor 
```
cd server
npm install 
npm run dev
```
Para correr el mock (le envía comandos para el tiempo y el puntaje, es a modo de prueba)
```
cd mock
node mock-micro.js
```

Para probarlo, en otra terminal 
```
curl -X POST -H "Content-Type: application/json" -d '{"score":0,"time_left":5}' http://localhost:5000/update-status
{"ok":true,"state":{"score":0,"time_left":5,"state":"running","level":1,"device"
:"mesa1","ts":"2025-10-06T21:03:01.814Z"}}
```



