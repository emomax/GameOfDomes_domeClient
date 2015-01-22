package global {
	public class Conversations {
		public static function get(progress:String) : Array {
			
			var convo:Array;
			
			switch (progress) {
				case "mike_intro":
					convo = new Array(
						["Mike", "Hey, we're finally here!\nIt's been ages since we last blasted some space targets!"],
						["Mike", "How about a little challenge?\nThe first one to shoot 5 space targets wins!"],
						["Mike", "Do you think you have what it takes to beat ol' Mike?"],
						["UA", "([W, A, S, D] to steer your vessel. \n[SPACE] to fire primary weapon.)"]);
					break;
				case "mike_intro_middle":
					convo = new Array(
						["Mike", "How's it going ol' friend? I have already shot 3!\nKeep it up!"]);
					break;
				case "mike_intro_over":
					convo = new Array(
						["Mike", "A proud count of 4! How are you doing?"],
						["Mike", "5 ALREADY?\nWow, you are good!"],
						["Mike", "Alright, I admit defeat, but next time I prom..."],
						["mike_shake", ""],
						["Mike", "What was that?"],
						["show_screecher", ""],
						["UA", "You have intruded our territory. For this you will die."],
						["Mike", "What is he talking about? He must be kid..."],
						["Mike", "AAARRRGGH!!!"],
						["mike_explode", ""], 
						["UA", "Puny humans.\nYou will suffer the same fate as your friend!"],
						["UA", "Fwuhuhuhu!"]);
					break;
				case "boss_mid_fight":
					convo = new Array(["UA", "Hrmf.. Weak human! \nMy power level is over 9000!"],
									  ["UA", "My shield is still above 70%!\nYou will never defeat me!"]);
					break;
				case "boss_new_weapon":
					convo = new Array(["UA", "Gah! My shield is beneath 50%!\nHow is this possible!?"],
									  ["UA", "Well well.. \nI guess I have to stop playing then."],
									  ["UA", "Now you will see the full power of the Voreenean Empire!"],
									  ["UA", "Taste my lasers, scum!"]);
					break;
				case "boss_leaving":
					convo = new Array(["UA", "ENOUGH!"],
									  ["UA", "I don't need to prove myself against scum like you!"],
									  ["UA", "We will meet again, and when we do.."],
									  ["UA", "..I will crush you like your puny friend."],
									  ["UA", "Hrmf..."]);
					break;
				case "commander_intro":
					convo = new Array(["Commander", "... So here we are. How are you feeling?"],
									  ["Commander", "Atleast the funeral was beautiful.\nPoor Mike.."],
									  ["Commander", ".. anyhow. I brought you out here for a reason.\nOutside enemy radars."],
									  ["Commander", "I want us to strike against the Voreenean home planet."],
									  ["Commander", "Attack them at their heart.\nKill them all."],
									  ["Commander", "Since we cannot figh..."],
									  ["commander_hit", ""],
									  ["Commander", "Blast it! Drones!"],
									  ["Commander", "How did they know we were here?"],
									  ["Commander", "Ready yourself! Avoid the enemy ships!"]);
					break;
				case "commander_middle":
					convo = new Array(["Commander", "You're still alive!\nI saw some Phasers up ahead!"],
									  ["Commander", "Let's hope you're as good with real enemies as with space targets.."],
									  ["Commander", "CHARGE FORTH!\nFOR MIKE!"]);
					break;
				case "commander_regroup":
					convo = new Array(["Commander", "It seems they are re-grouping!"],
						["Commander", "Take cover!"]);
					break;
				case "commander_combo":
					convo = new Array(["Commander", "The drones are gone!"],
									  ["Commander", "Finish the remaining Phasers so we can\n get out of here!"]);
					break;
				case "commander_finished":
					convo = new Array(["Commander", "Well fought!\nThat'll teach them not to mess with the human race!"],
									  ["Commander", "Let's head back to the base before more of these filth arrive."])
					break;
				case "commander_intro_2":
					convo = new Array(["Commander", "Good job back there.\nWe sure showed them what they're up against!"],
									  ["Commander", "Here we are, at the Voreenean home planet..."],
									  ["Commander", "... but where are they all?"],
									  ["show_enemies", ""],
									  ["Commander", "Whaaah..!"],
									  ["UA", "It has come to my attention that you\nare two whereas I am only one."],
									  ["UA", "What would happen if we simply..."],
									  ["UA", "... would even out the odds?"],
									  ["Commander", "No, wait!!!"],
									  ["kill_commander", ""],
									  ["UA", "And now, the small one.."],
									  ["UA", "Did you really think you could venture\nto the Voreenean home planet and make it out alive?."], 
									  ["UA", "Fwuhuhuhuhu..."]);
					break;
				case "boss_2_phase_1":
					convo = new Array(["UA", "Do you really think you have what it takes\nto beat the might of the Voreenean fleet?"]);
					break;
				case "boss_2_phase_2":
					convo = new Array(["UA", "MmMWhHrWWwmMmRRAGH!\n"],
									  ["UA", "DRONES! BRING ME HIS HEAD!"]);
					break;
				case "boss_2_phase_3":
					convo = new Array(["UA", "This.. This can't BE!"],
									  ["UA", "I'LL MAKE YOUR LIFE A LIVING HELL."]);
					break;
				default: 
					throw new Error("Couldn't not extract conversation - unknown progress: " + progress);
			}
			
			return convo;
		}
	}
}