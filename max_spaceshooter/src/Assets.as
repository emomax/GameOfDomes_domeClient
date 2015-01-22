package
{
	import flash.display.Bitmap;
	import flash.utils.Dictionary;
	
	import starling.textures.Texture;
	import starling.textures.TextureAtlas;
	
	public class Assets
	{
		private static var sTextures:Dictionary = new Dictionary();
		private static var sTextureAtlas:TextureAtlas;
		
		// GRAPHICS
		
		[Embed(source="../media/graphics/ending_credits.png")]
		public static const EndingCredits:Class;
		
		[Embed(source="../media/graphics/MenuImage.png")]
		public static const MenuImage:Class;
		
		[Embed(source="../media/graphics/lvlSelection.png")]
		public static const LevelSelectionImage:Class;
		
		[Embed(source="../media/graphics/level_complete.png")]
		public static const LevelCompleteImage:Class;
		
		// LEVEL 2
		[Embed(source="../media/graphics/spaceBackground_2.png")]
		public static const Background_2:Class;
		
		[Embed(source="../media/graphics/spaceForeground_2.png")]
		public static const Foreground_2:Class;
		
		[Embed(source="../media/graphics/spaceForeground2_2.png")]
		public static const Foreground2_2:Class;		
		
		// LEVEL 1
		[Embed(source="../media/graphics/spaceBackground.png")]
		public static const Background:Class;
		
		[Embed(source="../media/graphics/spaceForeground.png")]
		public static const Foreground:Class;
		
		[Embed(source="../media/graphics/spaceForeground2.png")]
		public static const Foreground2:Class;
		
		// LEVEL 3
		[Embed(source="../media/graphics/spaceBackground_3.png")]
		public static const Background_3:Class;
		
		[Embed(source="../media/graphics/spaceForeground_3.png")]
		public static const Foreground_3:Class;
		
		[Embed(source="../media/graphics/spaceForeground2_3.png")]
		public static const Foreground2_3:Class;
		
		[Embed(source="../media/graphics/atlas.png")]
		public static const AtlasTextureGame:Class;
		
		[Embed(source="../media/graphics/atlas.xml", mimeType="application/octet-stream")]
		public static const AtlasXmlGame:Class;
		
		// FONTS
		
		[Embed(source="../fonts/Eras\ Demi\ ITC.ttf", embedAsCFF="false", mimeType = "application/x-font",fontFamily="Eras Sans", advancedAntiAliasing = "true", unicodeRange = "U+0020-U+007e")]
		public static const Eras:Class;
		
		// SOUNDS
		
		[Embed(source="../media/sounds/shieldGeneratorHit.mp3")] 
		public static const ShieldHitSound:Class; 
		
		[Embed(source="../media/sounds/basiclazer2.mp3")]
		public static const LaserSound:Class;
		
		[Embed(source="../media/sounds/explosion_1_2.mp3")]
		public static const ExplosionSound:Class;
		
		[Embed(source="../media/sounds/blipSound.mp3")]
		public static const TypingSound:Class;
		
		[Embed(source="../media/sounds/play.mp3")]
		public static const PlaySound:Class;
		
		[Embed(source="../media/sounds/start.mp3")]
		public static const ClickSound:Class;
		
		// MUSIC
		
		[Embed(source="../media/sounds/lvl_complete.mp3")]
		public static const CompleteMusic:Class;
		
		[Embed(source="../media/sounds/lvl_fanfare.mp3")]
		public static const FanfareMusic:Class;
		
		[Embed(source="../media/sounds/boss_fight.mp3")]
		public static const BossMusic:Class;
		
		[Embed(source="../media/sounds/lounge.mp3")]
		public static const MenuMusic:Class;
		
		[Embed(source="../media/sounds/lvl2.mp3")]
		public static const Level2Music:Class;
		
		[Embed(source="../media/sounds/gameOver.mp3")]
		public static const GameOverMusic:Class;
		
		public static function getAtlas():TextureAtlas {
			if (sTextureAtlas == null) {
				var texture:Texture = getTexture("AtlasTextureGame");
				var xml:XML = XML(new AtlasXmlGame());
				sTextureAtlas = new TextureAtlas(texture, xml); 
			}
			return sTextureAtlas;
		}
		
		public static function getTexture(name:String):Texture
		{
			if (sTextures[name] == undefined)
			{	
				var bitmap:Bitmap = new Assets[name]();
				sTextures[name] = Texture.fromBitmap(bitmap);
			}
			
			return sTextures[name];
		}
	}
}
