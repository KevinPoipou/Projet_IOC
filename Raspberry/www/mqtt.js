var ipAddress = "192.168.137.133";


function publishMessage(topic, message) {
    console.log("Message publié :", topic, message);
  
    // Create a client instance
    var client = mqtt.connect('ws://' + ipAddress + ':9001'); // 9001 car je suis en web
  
    // Callbacks
    client.on('connect', function () {
      console.log('Connected to MQTT broker');
      // Publish message
      client.publish(topic, message);
      console.log('Message published');
      // Disconnect after publishing
      client.end();
    });
  
    client.on('error', function (error) {
      console.error('Error:', error);
    });
  }

  function subscribeMessage(topic) {
    // Create a client instance
    var client = mqtt.connect(('ws://' + ipAddress + ':9001')); 
    
    // Callbacks
    client.on('connect', function () {
      console.log('Connected to MQTT broker');
      // Subscribe to topic
      client.subscribe(topic, function (err) {
        if (err) {
          console.error('Error subscribing to topic:', err);
        } else {
          console.log('Subscribed to topic:', topic);
        }
      });
    });

    client.on('message', function (receivedTopic, message) {
      // Handle received message
      console.log('Received message from topic:', receivedTopic);
      console.log('Message:', message.toString());
      var parsedWebMessage = JSON.parse(message);
      var progress = parsedWebMessage.progress;
      var mac_address = parsedWebMessage.mac_address;
      updateProgressBar(progress, mac_address);
    });

    client.on('error', function (error) {
      console.error('Error:', error);
    });
  }

  subscribeMessage('web');
