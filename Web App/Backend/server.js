const express = require('express');
const mqtt = require('mqtt');
const bodyParser = require('body-parser');
const fs = require('fs');
const path = require('path'); 

const app = express();
app.use(bodyParser.json());

// ✅ Serve the frontend folder statically
app.use(express.static(path.join(__dirname, '../Frontend')));

// MQTT TLS config
const options = {
  host: 'je1b6b22.ala.eu-central-1.emqxsl.com',
  port: 8883,
  protocol: 'mqtts',
  username: 'dairo',
  password: 'Dairo2828',
  ca: fs.readFileSync("D:\\Prog\\IOT\\MQVoice\\Certificate\\emqxsl-ca.crt") 
};

const client = mqtt.connect(options);

client.on('connect', () => {
  console.log('✅ Connected to EMQX broker');
  console.log('------------------------------------------');
});

app.use(bodyParser.json());

// === Post + Get Handler ===
function handleLedRequest(req, res, state) {
  const normalized = state?.toLowerCase();
  if (normalized === 'on' || normalized === 'off') {
    const payload = normalized.toUpperCase();
    client.publish('led/control', payload);
    console.log(`🔁 LED state sent: ${payload}`);
    res.send(`✅ LED turned ${normalized}`);
  } else {
    console.warn('⚠️ Invalid LED state received:', state);
    res.status(400).send('❌ Invalid state. Use "on" or "off".');
  }
  console.log('------------------------------------------');
}

// Handle POST
app.post('/led', (req, res) => {
  const state = req.body.state;
  handleLedRequest(req, res, state);
});

// Handle GET : /led?state=on
app.get('/led', (req, res) => {
  const state = req.query.state;
  handleLedRequest(req, res, state);
});

app.listen(3000, () => console.log('🌐 Listening on port 3000'));
