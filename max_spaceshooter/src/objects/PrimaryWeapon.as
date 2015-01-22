package objects
{
	import flash.utils.Timer;

	public interface PrimaryWeapon
	{
		function get fireSpeedPrimary() : Number; // fireSpeed in ms
		function get fireTimerPrimary() : Timer;
		function get canFirePrimary() : Boolean;
		function get firePowerPrimary() : int;

		function set fireSpeedPrimary(n:Number) : void; // fireSpeed in ms
		function set fireTimerPrimary(t:Timer) : void;
		function set canFirePrimary(b:Boolean) : void;
		function set firePowerPrimary(i:int) : void;
		
		function primary() : void;
	}
}