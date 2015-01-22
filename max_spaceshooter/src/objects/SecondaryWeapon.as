package objects
{
	import flash.utils.Timer;

	public interface SecondaryWeapon
	{
		
		function get fireSpeedSecondary() : Number; // fireSpeed in ms
		function get fireTimerSecondary() : Timer;
		function get canFireSecondary() : Boolean;
		function get firePowerSecondary() : int;
		
		function set fireSpeedSecondary(n:Number) : void; // fireSpeed in ms
		function set fireTimerSecondary(t:Timer) : void;
		function set canFireSecondary(b:Boolean) : void;
		function set firePowerSecondary(i:int) : void;
		
		function secondary() : void;
	}
}