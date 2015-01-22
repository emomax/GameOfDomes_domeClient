package objects {
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;

	public class IngameBackground extends Sprite{
		
		public var bg:Image;
		public var bg2:Image;
		
		public var fg:Image;
		public var fg2:Image;
		
		public var mg:Image;
		public var mg2:Image;
		
		private var speed:int = 1;
		private var stageRef:Sprite;
		
		private var bgX:Number = 0;
		
		public function IngameBackground(s:Sprite, level:int) {
			stageRef = s;
			
			switch (level) {
				case 1:
					bg = new Image(Assets.getTexture("Background"));
					bg2 = new Image(Assets.getTexture("Background"));
					
					fg = new Image(Assets.getTexture("Foreground"));
					fg2 = new Image(Assets.getTexture("Foreground"));
					
					mg = new Image(Assets.getTexture("Foreground2"));
					mg2 = new Image(Assets.getTexture("Foreground2"));
					break;
				case 2:
					bg = new Image(Assets.getTexture("Background_2"));
					bg2 = new Image(Assets.getTexture("Background_2"));
					
					fg = new Image(Assets.getTexture("Foreground_2"));
					fg2 = new Image(Assets.getTexture("Foreground_2"));
					
					mg = new Image(Assets.getTexture("Foreground2_2"));
					mg2 = new Image(Assets.getTexture("Foreground2_2"));
					break;
				case 3:
					bg = new Image(Assets.getTexture("Background_3"));
					bg2 = new Image(Assets.getTexture("Background_3"));
					
					fg = new Image(Assets.getTexture("Foreground_3"));
					fg2 = new Image(Assets.getTexture("Foreground_3"));
					
					mg = new Image(Assets.getTexture("Foreground2_3"));
					mg2 = new Image(Assets.getTexture("Foreground2_3"));
			}
			
			bg2.x = -700;
			mg2.x = -700;
			fg2.x = -700;
			
			addChild(bg);
			addChild(bg2);
			
			addChild(mg);
			addChild(mg2);
			
			addChild(fg);
			addChild(fg2);
			
			addEventListener(Event.ENTER_FRAME, backgroundLoop);
		}
		
		private function backgroundLoop(e:Event) : void {
			
			
			fg.x += 10 * speed;
			fg2.x += 10 * speed;
			
			mg.x += 4 * speed;
			mg2.x += 4 * speed;
			
			bgX += speed;
			
			bg.x = Math.ceil(bgX);
			bg2.x = Math.ceil(bgX) - 700;
			
			
			if (fg.x > 700) {
				fg.x -= 700;
				fg2.x -= 700;
			}
			
			if (bg.x > 700) {
				bg.x -= 700;
				bg2.x -= 700;
				bgX = 0;
			}
			
			if (mg.x > 700) {
				mg.x -= 700;
				mg2.x -= 700;
			}
		}
	}
}