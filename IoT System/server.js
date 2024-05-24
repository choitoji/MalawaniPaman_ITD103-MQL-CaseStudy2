
const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Connect to MongoDB
mongoose.connect('mongodb://localhost:27017/audioLoudness', {
  useNewUrlParser: true,
  useUnifiedTopology: true
});

// Define schema and model
const audioSchema = new mongoose.Schema({
  timestamp: { type: Date, default: Date.now },
  level: Number
});

const Audio = mongoose.model('Audio', audioSchema);

// Middleware
app.use(cors({
  origin: 'http://localhost:3001'  // Adjust this to match your frontend's URL
}));
app.use(bodyParser.json());

// Routes
app.get('/', (req, res) => {
  res.send('Audio Loudness API is running');
});

app.get('/api/audio', async (req, res) => {
  try {
    const data = await Audio.find().sort({ timestamp: -1 }).limit(100);
    res.json(data);
  } catch (error) {
    res.status(500).send(error);
  }
});

app.post('/api/audio', async (req, res) => {
  try {
    const audio = new Audio(req.body);
    await audio.save();
    res.status(201).send(audio);
  } catch (error) {
    res.status(500).send(error);
  }
});

// Start server
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

