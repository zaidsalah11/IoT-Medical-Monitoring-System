% 1. Channel and Keys Setup
channelID = 3282384; 
readKey = 'LIDCV1J6VZPTYJY0'; 
alertKey = 'TAKoq0GvonBXi2DEPjw'; 

% 2. Get data as Matrix (Verified Logic)
data = thingSpeakRead(channelID, 'ReadKey', readKey, 'NumPoints', 1, 'OutputFormat', 'Matrix');

% 3. Check data and send Integrated Report
if ~isempty(data)
    temp = data(1); hum = data(2); gas = data(3); dist = data(4);
    
    alertUrl = 'https://api.thingspeak.com/alerts/send';
    options = weboptions('HeaderFields', {'ThingSpeak-Alerts-API-Key', alertKey});
    
    subject = 'Integrated Medical Report: Status Alert';
    body = sprintf(['Medical Alert! System detected abnormal values:\n\n', ...
                    '1. Temp: %.1f C\n', ...
                    '2. Humidity: %.1f %%\n', ...
                    '3. Gas Level: %.0f\n', ...
                    '4. Distance: %.1f cm\n\n', ...
                    'Check patient status immediately.'], temp, hum, gas, dist);

    webwrite(alertUrl, 'body', body, 'subject', subject, options);
end
