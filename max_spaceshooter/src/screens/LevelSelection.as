package screens {
	
	import flash.geom.Point;
	import flash.text.Font;
	
	import events.NavigationEvent;
	
	import global.VARS;
	
	import starling.display.Button;
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.text.TextField;
	import starling.utils.deg2rad;
	
	public class LevelSelection extends Sprite {
		
		private var bg:Image = new Image(Assets.getTexture("LevelSelectionImage"))
		
		private var level1:Button;
		private var level2:Button;
		private var level3:Button;
		
		private var label1:TextField;
		private var label2:TextField;
		private var label3:TextField;
		
		private var origin1:Point;
		private var origin2:Point;
		private var origin3:Point;
		
		private var levels:Array;
		private var origins:Array;
		
		private var header:TextField;
		
		private var bodyFont:Font = new Assets.Eras;
		
		private var iterator:int = 0;
		
		public function LevelSelection() : void {
			super();
			
		}
		
		public function init() : void {
			origins = new Array();
			levels = new Array();
			createGraphics();
		}
		
		override public function dispose() : void {
			removeListeners();
			removeGraphics();
			emptyArrays();
		}
		
		private function emptyArrays() : void {
			while (levels.length != 0) {
				levels.splice(0, 1);
			}
			
			while (origins.length != 0) {
				origins.splice(0, 1);
			}
		}
		
		
		private function removeGraphics() : void {
			if (contains(bg)) removeChild(bg, true);
			
			for (var i:uint = 0; i < levels.length; ++i) {
				if (contains(levels[i])) removeChild(levels[i]);
			}
		}
		
		private function removeListeners() : void {
			if (hasEventListener(Event.ENTER_FRAME)) {
				removeEventListener(Event.ENTER_FRAME, planetRotation);
			}
			
			for (var i:uint = 0; i < levels.length; ++i) {
				if (levels[i].hasEventListener(Event.TRIGGERED)) {
					levels[i].removeEventListener(Event.TRIGGERED, onClick);
				}
			}
		}
		
		private function createGraphics() : void {
			this.addChild(bg);
			
			// instantiate buttons
			level1 = new Button(Assets.getAtlas().getTexture("planet_1"));
			level2 = new Button(Assets.getAtlas().getTexture("planet_2"));
			level3 = new Button(Assets.getAtlas().getTexture("planet_3"));
			
			origin1 = new Point(55, 388);
			origin2 = new Point(128, 140);
			origin3 = new Point(490, 262);
			
			level1.x = origin1.x;
			level1.y = origin1.y;
			
			level2.x = origin2.x;
			level2.y = origin2.y;
			
			level3.x = origin3.x;
			level3.y = origin3.y;
			
			levels.push(level1);
			levels.push(level2);
			levels.push(level3);
			
			origins.push(origin1);
			origins.push(origin2);
			origins.push(origin3);
			
			// Extract levels to be highlighted
			for (var i:uint = 0; i < VARS.levelProgress; ++i) {
				levels[i].addEventListener(Event.TRIGGERED, onClick);
				addChild(levels[i]);
			}
			
			for (var j:uint = VARS.levelProgress; j < levels.length; j++) {
				levels[j].upState = Assets.getAtlas().getTexture("planet_"+ (j + 1) + "_ua");
				levels[j].downState = Assets.getAtlas().getTexture("planet_"+ (j + 1) + "_ua");
				(levels[j] as Button).enabled = false;
				addChild(levels[j]);
			}
			
			addEventListener(Event.ENTER_FRAME, planetRotation);
		}
		
		private function planetRotation(e:Event) : void {
			iterator += 3;

			for (var i:uint = 0; i < origins.length; ++i) {
				levels[i].x = 2*Math.cos(deg2rad(iterator)) + (origins[i] as Point).x;
				levels[i].y = 2*Math.sin(deg2rad(iterator)) + (origins[i] as Point).y;
			}
		}
		
		private function onClick(e:Event) : void {
			switch (e.target) {
				case level1:
					dispose();
					this.dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "level_1"}, true));
					break;
				case level2:
					dispose();
					this.dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "level_2"}, true));
					break;
				case level3:
					dispose();
					this.dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "level_3"}, true));
					break;
			}
		}
	}
}