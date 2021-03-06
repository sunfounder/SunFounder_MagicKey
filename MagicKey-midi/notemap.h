// int currentVelocity means velocity parameters is 0~127, you can set yourself.
// NOTE_C, NOTE_D, the values behind them are tone values, you can modify them refering to note number list
// Note Number List，shows the tone numbers

#define CHANNEL_KEYBOARD 0  // use keyboard CHANNEL
#define CHANNEL_DRUM 9      // use drum CHANNEL
#define CONTROL_VOLUME  7   // Continuous Controllers, 7 means to change channel
int currentVelocity = 127;  // Velocity parameters range from 0 to 127
// for more information about midi control parameters, please refer to http://nickfever.com/music/midi-cc-list

#ifdef KEYBOARD
	int channel = CHANNEL_KEYBOARD;  // select different instrument channel with different midi values
	// keyboard note
	#define NOTE_C   60   // C5
	#define NOTE_D   62   // D5
	#define NOTE_E   64   // E5
	#define NOTE_F   65   // F5
	#define NOTE_G   67   // G5
	#define NOTE_A   69   // A5
	#define NOTE_B   71   // B5
	#define NOTE_CP  72   // C6
#endif

#ifdef DRUM
	int channel = CHANNEL_DRUM;
	// drum note
	#define NOTE_C   56   // Hi-Hat
	#define NOTE_D   61   // Crash
	#define NOTE_E   52   // Snare
	#define NOTE_F   53   // Tom
	#define NOTE_G   48   // Kick
	#define NOTE_A   59   // Tom 2
	#define NOTE_B   63   // Ride
	#define NOTE_CP  43   // Floor Tom
#endif

// Note Number List:
// | Octave |                              Note Numbers                             |
// |--------|-----------------------------------------------------------------------|
// |        | C   | C#  | D   | D#  | E   | F   | F#  | G   | G#  | A   | A#  | B   |
// |   0    | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  |
// |   1    | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  | 22  | 23  |
// |   2    | 24  | 25  | 26  | 27  | 28  | 29  | 30  | 31  | 32  | 33  | 34  | 35  |
// |   3    | 36  | 37  | 38  | 39  | 40  | 41  | 42  | 43  | 44  | 45  | 46  | 47  |
// |   4    | 48  | 49  | 50  | 51  | 52  | 53  | 54  | 55  | 56  | 57  | 58  | 59  |
// |   5    | 60  | 61  | 62  | 63  | 64  | 65  | 66  | 67  | 68  | 69  | 70  | 71  |
// |   6    | 72  | 73  | 74  | 75  | 76  | 77  | 78  | 79  | 80  | 81  | 82  | 83  |
// |   7    | 84  | 85  | 86  | 87  | 88  | 89  | 90  | 91  | 92  | 93  | 94  | 95  |
// |   8    | 96  | 97  | 98  | 99  | 100 | 101 | 102 | 103 | 104 | 105 | 106 | 107 |
// |   9    | 108 | 109 | 110 | 111 | 112 | 113 | 114 | 115 | 116 | 117 | 118 | 119 |
// |   10   | 120 | 121 | 122 | 123 | 124 | 125 | 126 | 127 |     |     |     |     |
