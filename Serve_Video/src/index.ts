import express from "express"

import httpProxy from "http-proxy";

const app = express()
const PORT = 8000

const BASE_PATH:string = 'https://pub-99108591c26945f880c380bf32c43a20.r2.dev/videodata'

const proxy = httpProxy.createProxy()

app.use((req, res) => {
    const hostname = req.hostname;
    const subdomain = hostname.split('.')[0];
    console.log("HOSTNAME", hostname)

    // Custom Domain - DB Query
    console.log("SUBDOMAIN", subdomain);

    const resolvesTo = `${BASE_PATH}/${subdomain}`;
    console.log("RESOLVES TO", resolvesTo)  

    return proxy.web(req, res, { target: resolvesTo, changeOrigin: true })

})

proxy.on('proxyReq', (proxyReq, req, res) => {
    const url = req.url;
    console.log(proxyReq.path + 'footage.mp4')
    console.log("URL", url);
   
    if (url === '/')
        proxyReq.path += 'footage.mp4';

})


app.listen(PORT, () => console.log("Reverse Proxy Running..${PORT}"))