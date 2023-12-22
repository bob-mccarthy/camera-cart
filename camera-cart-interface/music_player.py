import vlc, time
p = vlc.MediaPlayer("/Users/robertmccarthy/Desktop/bent.mp3")
p.play()
p.set_time(70000)
p.set_rate(1)
time.sleep(10)

