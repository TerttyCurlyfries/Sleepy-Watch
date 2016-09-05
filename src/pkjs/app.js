var sleepyRequest = function (url, type, callback) {
    var sleepytitle = new XMLHttpRequest();
    sleepytitle.onload = function () {
    callback(this.responseText);
  };
  sleepytitle.open(type, url);
  sleepytitle.send();
  };

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    console.log('Getting latest Sleepycast data...');
  
// Send request to soundcloud
  sleepyRequest('http://feeds.soundcloud.com/users/soundcloud:users:108438041/sounds.rss', 'GET', 
    function(responseText) {
      // responseText contains a JSON object with TITLE data
      var titlestart = responseText.search('SleepyCast S');
      var titleend = responseText.search(' - \\[');
      var title = responseText.substring(titlestart,titleend);
      console.log(title);
      
      // EPISODE data
      var episodestart = responseText.search('\\[');
      var episodeend = responseText.search(']</title>');
      var episode = responseText.substring(episodestart,episodeend + 1);
      console.log(episode);
      
      // HOSTS data
      var hostsstart = responseText.search('<itunes:summary>');
      //var hostsend = responseText.search('+');
      var hosts = responseText.substring(hostsstart + 16,hostsstart + 100) + '...';
      console.log(hosts);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TITLE": title,
        "KEY_EPISODE": episode,
        "KEY_HOSTS": hosts
      };
      console.log("Dictionary constructed!");
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Title info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending title info to Pebble!");
        }
      );
    }                   
  );
  }
);

